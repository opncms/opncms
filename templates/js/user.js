/**
 * user, JavaScript User Interface
 *
 * @version 0.0.1
 * @license GNU General Public License, http://www.gnu.org/licenses/gpl.txt
 * @author  kpeo, kpeo.y3k@gmail.com
 * @created 2015-05-15
 * @updated 2015-05-15
 * @link    http://opnproject.org/dp
 * @depend  -
 */

var user = {
        version : "0.0.1",
        rpc_url : "/u_rpc",
        binding : true, // automatic binding
        bindId : 'user', // class name
        preloading : true,

        RPC : null,

        install : function() {
                          user.addEvent(window, 'load', user.init);
        },

        init : function()
	{
		var el;

                if(user.binding) {
                        user.bind();
                }
		user.rpc_init(user.rpc_url);


		el = document.getElementById('button-signin');
		if(el)
		{
                	el.onclick = function(e) {
				user.rpc_signin();
                        	return false;
			}
                }
		el = document.getElementById('button-signout');
		if(el)
		{
			el.onclick = function(e)
			{
				user.rpc_signout();
				return false;
			}
		}
        },
        bind : function() {
                //var e = document.getElementById(bindId);
        },

        addEvent : function(el, evnt, func) {
                if(el.addEventListener) {
                        el.addEventListener(evnt, func, false);
                } else if(el.attachEvent) {
                        el.attachEvent('on'+evnt, func);
                }
        },

        rpc_init: function(res) {
                try {
                        user.RPC = new JsonRpc.ServiceProxy(res, {
                                asynchronous: true,  //default value, but if otherwise error raised
                                sanitize: false,     //explicit false required, otherwise error raised
				methods: ['signin','signout','signup'],
				callbackParamName: 'callback'
                        });
			JsonRpc.setAsynchronous(user.RPC, true);
                }
                catch(e){
                        alert('Please, add `json-xml-rpc` JavaScript!');
                }
        },
	rpc_signin: function() {
		var email = document.getElementById('inputEmail').value;
		var password = document.getElementById('inputPassword').value;

		JsonRpc.setAsynchronous(user.RPC, true);
		user.RPC.signin({params:[ email, password ],
			onSuccess:function(resultObj)
			{
			},
			onException:function(errorObj)
			{
				document.getElementById('signin-alert').style.display = "block";
			},
                        onComplete:function(responseObj)
			{
				if(responseObj && responseObj.error == null && responseObj.result == "ok") {
					if(window.location.href.indexOf("#")>0)
						window.location.href = window.location.href.split("#")[0];
					setTimeout(location.reload(true), 1000);
                        	}
				else
				{
					document.getElementById('signin-alert').style.display = "block";
				}
			}
		});
	},
        rpc_signout: function() {
		JsonRpc.setAsynchronous(user.RPC, true);
                user.RPC.signout({params:[ ],
                        onException:function(errorObj){
                                alert("Exception: " + errorObj);
                        },
                        onComplete:function(responseObj){
				if(responseObj && responseObj.error == null && responseObj.result == "ok") {
					if(window.location.href.indexOf("#")>0)
                                        	window.location.href = window.location.href.split("#")[0];
					setTimeout(location.reload(true),1000);
				}
                        }
                });
        }

};

user.install();
