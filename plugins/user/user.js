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
        rpc : "/u_rpc",
        binding : true, // automatic binding
        bindId : 'user', // class name
        preloading : true,

        dpRPC : null,

        install : function() {
                          user.addEvent(window, 'load', user.init);
        },

        init : function() {

                if(user.binding) {
                        user.bind();
                }
                document.getElementById('button-singin').onmousedown = function(e) 		{
                        var evt = window.event || e;
                        return;
                }
                user.rpc_init(user.rpc,1);
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

        rpc_init: function(res, async) {
                try {
                        dayplan.dpRPC = new rpc.ServiceProxy(res, {
                                asynchronous: (async ? true : false),  //default value, but if otherwise error raised
                                sanitize: false     //explicit false required, otherwise error raised
                        });
                }
                catch(e){
                        alert('Please, add `json-xml-rpc` JavaScript!');
                }
        },

};

dayplan.install();
