#include <opncms/esmtp.h>
	   
namespace ESMTP
{
	static enum notify_flags notify_;
	static struct sigaction sa_;
	static smtp_session_t session_;
	static smtp_message_t message_;
	static smtp_recipient_t recipient_;
	static auth_context_t authctx_;
	static const smtp_status_t *status_;
	static std::string user_;
	static std::string password_;
	static std::string tls_password_;

bool open(const std::string& host, const std::string& user, const std::string& password, const std::string& tls_password)
{
	BOOSTER_LOG(debug,__FUNCTION__);
	notify_ = Notify_NOTSET;
	user_ = user;
	password_ = password;
	tls_password_ = tls_password;
	
	/* NB.  libESMTP sets timeouts as it progresses through the protocol.
	In addition the remote server might close its socket on a timeout.
	Consequently libESMTP may sometimes try to write to a socket with
	no reader.  Ignore SIGPIPE, then the program doesn't get killed
	if/when this happens. */
	sa_.sa_handler = SIG_IGN;
	sigemptyset(&sa_.sa_mask);
	sa_.sa_flags = 0;
	sigaction(SIGPIPE, &sa_, NULL); 

	auth_client_init();

	if ( !(session_ = smtp_create_session()) ) {
		BOOSTER_LOG(error,__FUNCTION__) << "Cannot create SMTP session";
		return false;
	}

	/* Set the host running the SMTP server.  LibESMTP has a default port
	number of 587, however this is not widely deployed so the port
	is specified as 25 along with the default MTA host. */
	smtp_set_server(session_, host.empty() ? ESMTP_HOST_DEFAULT : host.c_str());

	smtp_set_monitorcb(session_, monitor_cb, stdout, 1);

    if ( !(user.empty() && password.empty()) )
	{
		authctx_ = auth_create_context();
		auth_set_mechanism_flags(authctx_, AUTH_PLUGIN_PLAIN, 0);
		auth_set_interact_cb(authctx_, authinteract, NULL);
	}

	smtp_starttls_enable(session_, Starttls_ENABLED); //Starttls_REQUIRED
	/* Use our callback for X.509 certificate passwords.  If STARTTLS is
	not in use or disabled in configure, the following is harmless. */
	if(!tls_password.empty() || 1)
	{
		smtp_starttls_set_password_cb(tlsinteract, NULL);
		smtp_set_eventcb(session_, event_cb, NULL);
	}
	/* Now tell libESMTP it can use the SMTP AUTH extension.*/
	//if authentication required
    if ( !smtp_auth_set_context(session_, authctx_))
	{
		BOOSTER_LOG(error,__FUNCTION__) << "Cannot set SMTP auth context";
		return false;
	}
	return true;
}

void close()
{
	BOOSTER_LOG(debug,__FUNCTION__);
	smtp_destroy_session(session_);
	auth_destroy_context(authctx_);
	auth_client_exit();
}

bool send(const std::string& from, const std::string& to, 
	std::vector<std::string>& recipients, 
	const std::string& subject, const std::string& data)
{
	BOOSTER_LOG(debug,__FUNCTION__);
	message_ = smtp_add_message(session_);
	/* Request MDN sent to the same address as the reverse path */
	//smtp_set_header(message, "Disposition-Notification-To", NULL, NULL);

	/* Set the reverse path for the mail envelope.  (NULL is ok) */
	smtp_set_reverse_path(message_, from.c_str());

#if 0
	/* The message-id is OPTIONAL but SHOULD be present.  By default
	libESMTP supplies one.  If this is not desirable, the following
	prevents one making its way to the server.
	N.B. It is not possible to prohibit REQUIRED headers.  Furthermore,
	the submission server will probably add a Message-ID header,
	so this cannot prevent the delivered message from containing
	the message-id.  */
	smtp_set_header_option(message_, "Message-Id", Hdr_PROHIBIT, 1);
#endif

	/* RFC 2822 doesn't require recipient headers but a To: header would
	be nice to have if not present. */
	if(!to.empty())
		smtp_set_header(message_, "To", NULL, NULL);
	else
		smtp_set_header(message_, "To", NULL, to.c_str());

	/* Set the Subject: header.  For no reason, we want the supplied subject
	to override any subject line in the message headers. */
	if (!subject.empty())
	{
		smtp_set_header(message_, "Subject", subject.c_str());
		smtp_set_header_option(message_, "Subject", Hdr_OVERRIDE, 1);
	}

	std::string msg = std::string("MIME-Version: 1.0\r\n") +
             "Content-Type: text/plain;\r\n" +
             "Content-Transfer-Encoding: 8bit\r\n" +
             "Subject: " + subject + "\r\n" +
             "\r\n" + 
             "Message: " + data;

	char s[ESMTP_MAX_STR];
	strcpy(s, msg.c_str());
	smtp_set_message_str(message_, s);
	/* Add remaining program arguments as message recipients. */
	std::vector<std::string>::const_iterator it;
	for(it = recipients.begin(); it != recipients.end(); ++it )
	{
		recipient_ = smtp_add_recipient(message_, it->c_str());

		/* Recipient options set here */
		if (notify_ != Notify_NOTSET)
			smtp_dsn_set_notify(recipient_, notify_);
	}
	/* Initiate a connection to the SMTP server and transfer the message. */
	if (!smtp_start_session(session_))
	{
		char buf[128];
		BOOSTER_LOG(error,__FUNCTION__) << "SMTP server problem: " << smtp_strerror(smtp_errno(), buf, sizeof buf);
		return false;
	}
	
	/* Report on the success or otherwise of the mail transfer. */
	status_ = smtp_message_transfer_status(message_);
	BOOSTER_LOG(debug,__FUNCTION__) << status_->code << ((status_->text != NULL) ? status_->text : "\n") << std::endl;
	smtp_enumerate_recipients (message_, print_recipient_status, NULL);
	return true;
}

/* Callback to prnt the recipient status */
void print_recipient_status(smtp_recipient_t recipient, const char *mailbox, void *arg arg_unused)
{
	const smtp_status_t *status;

	status = smtp_recipient_status(recipient);
	BOOSTER_LOG(debug,__FUNCTION__) << mailbox << ": " << status->code << " " << status->text;
}

/* Callback function to read the message from a file.  Since libESMTP
   does not provide callbacks which translate line endings, one must
   be provided by the application.

   The message is read a line at a time and the newlines converted
   to \r\n.  Unfortunately, RFC 822 states that bare \n and \r are
   acceptable in messages and that individually they do not constitute a
   line termination.  This requirement cannot be reconciled with storing
   messages with Unix line terminations.  RFC 2822 rescues this situation
   slightly by prohibiting lone \r and \n in messages.

   The following code cannot therefore work correctly in all situations.
   Furthermore it is very inefficient since it must search for the \n.
 */
void monitor_cb(const char *buf, int buflen, int writing, void *arg arg_unused)
{
	if (writing == SMTP_CB_HEADERS)
	{
		BOOSTER_LOG(debug,__FUNCTION__) << "== "  << std::string(buf, buflen);
		return;
	}
	BOOSTER_LOG(debug,__FUNCTION__) << (writing ? ">> " : "<< ") << std::string(buf, buflen);
/*
	if (buf[buflen - 1] != '\n')
		BOOSTER_LOG(debug,__FUNCTION__);
*/
}

/* Callback to request user/password info.  Not thread safe. */
int authinteract(auth_client_request_t request, char **result, int fields, void *arg arg_unused)
{
	BOOSTER_LOG(debug,__FUNCTION__);
	char s[ESMTP_MAX_STR] = {0};
	
	for (int i = 0; i < fields; i++)
	{
		if(request[i].flags & AUTH_USER)
		{
			strcpy(s, user_.c_str());
			result[i] = s;
		}
		else if(request[i].flags & AUTH_PASS)
		{
			strcpy(s, password_.c_str());
			result[i] = s;
		}
		else
			return 0;
	}
	return 1;
}

int tlsinteract(char *buf, int buflen, int rwflag arg_unused, void *arg arg_unused)
{
	BOOSTER_LOG(debug,__FUNCTION__);
	int len = tls_password_.size();//strlen(tls_password_.c_str());
	if(len+1 > buflen)
		return 0;
	strcpy(buf, tls_password_.c_str());
	return len;
}

int handle_invalid_peer_certificate(long result)
{
	const char *k ="UNKNOWN";
	switch(result)
	{
		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
			k="X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT"; break;
		case X509_V_ERR_UNABLE_TO_GET_CRL:
			k="X509_V_ERR_UNABLE_TO_GET_CRL"; break;
		case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
			k="X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE"; break;
		case X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE:
			k="X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE"; break;
		case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
			k="X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY"; break;
		case X509_V_ERR_CERT_SIGNATURE_FAILURE:
			k="X509_V_ERR_CERT_SIGNATURE_FAILURE"; break;
		case X509_V_ERR_CRL_SIGNATURE_FAILURE:
			k="X509_V_ERR_CRL_SIGNATURE_FAILURE"; break;
		case X509_V_ERR_CERT_NOT_YET_VALID:
			k="X509_V_ERR_CERT_NOT_YET_VALID"; break;
		case X509_V_ERR_CERT_HAS_EXPIRED:
			k="X509_V_ERR_CERT_HAS_EXPIRED"; break;
		case X509_V_ERR_CRL_NOT_YET_VALID:
			k="X509_V_ERR_CRL_NOT_YET_VALID"; break;
		case X509_V_ERR_CRL_HAS_EXPIRED:
			k="X509_V_ERR_CRL_HAS_EXPIRED"; break;
		case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
			k="X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD"; break;
		case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
			k="X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD"; break;
		case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD:
			k="X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD"; break;
		case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD:
			k="X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD"; break;
		case X509_V_ERR_OUT_OF_MEM:
			k="X509_V_ERR_OUT_OF_MEM"; break;
		case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
			k="X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT"; break;
		case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
			k="X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN"; break;
		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
			k="X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY"; break;
		case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
			k="X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE"; break;
		case X509_V_ERR_CERT_CHAIN_TOO_LONG:
			k="X509_V_ERR_CERT_CHAIN_TOO_LONG"; break;
		case X509_V_ERR_CERT_REVOKED:
			k="X509_V_ERR_CERT_REVOKED"; break;
		case X509_V_ERR_INVALID_CA:
			k="X509_V_ERR_INVALID_CA"; break;
		case X509_V_ERR_PATH_LENGTH_EXCEEDED:
			k="X509_V_ERR_PATH_LENGTH_EXCEEDED"; break;
		case X509_V_ERR_INVALID_PURPOSE:
			k="X509_V_ERR_INVALID_PURPOSE"; break;
		case X509_V_ERR_CERT_UNTRUSTED:
			k="X509_V_ERR_CERT_UNTRUSTED"; break;
		case X509_V_ERR_CERT_REJECTED:
			k="X509_V_ERR_CERT_REJECTED"; break;
	}
	BOOSTER_LOG(error,__FUNCTION__) << "SMTP_EV_INVALID_PEER_CERTIFICATE: " << result << " " << k;
	return 1;
}

void event_cb(smtp_session_t session arg_unused, int event_no, void *arg,...)
{
	va_list alist;
	int *ok;

	va_start(alist, arg);

	switch(event_no)
	{
		case SMTP_EV_CONNECT: 
		case SMTP_EV_MAILSTATUS:
		case SMTP_EV_RCPTSTATUS:
		case SMTP_EV_MESSAGEDATA:
		case SMTP_EV_MESSAGESENT:
		case SMTP_EV_DISCONNECT: break;
		case SMTP_EV_WEAK_CIPHER: {
			int bits;
			bits = va_arg(alist, long); ok = va_arg(alist, int*);
			BOOSTER_LOG(debug,__FUNCTION__) << "SMTP_EV_WEAK_CIPHER, bits=" << bits << ". Accepted.";
			*ok = 1; break;
		}
		case SMTP_EV_STARTTLS_OK:
			BOOSTER_LOG(debug,__FUNCTION__) << "SMTP_EV_STARTTLS_OK - TLS started here.";
			break;
		case SMTP_EV_NO_PEER_CERTIFICATE: {
			ok = va_arg(alist, int*); 
			BOOSTER_LOG(debug,__FUNCTION__) << "SMTP_EV_NO_PEER_CERTIFICATE. Accepted.";
			*ok = 1; break;
		}
		case SMTP_EV_NO_CLIENT_CERTIFICATE: {
			ok = va_arg(alist, int*);
			BOOSTER_LOG(debug,__FUNCTION__) << "SMTP_EV_NO_CLIENT_CERTIFICATE. Accepted.";
			*ok = 1; break;
		}
		default:
			BOOSTER_LOG(debug,__FUNCTION__) << "Got event: " << event_no << ". Ignored.";
	}
	va_end(alist);
}

}
