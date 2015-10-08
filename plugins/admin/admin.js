/**
 * admin, JavaScript Admin Interface
 *
 * @version 0.0.1
 * @license GNU General Public License, http://www.gnu.org/licenses/gpl-3.0.html
 * @author  kpeo, opncms@gmail.com
 * @created 2015-05-15
 * @updated 2015-10-07
 * @link    http://opncms.com
 * @depend  -
 */

var admin = {
	version : "0.0.1",
	rpc_url : "/admin_rpc",
	binding : true, // automatic binding
	bindId : 'admin', // class name
	preloading : true,

	RPC : null,

	install : function() {
		admin.addEvent(window, 'load', admin.init);
	},

	init : function()
	{
		var el;
		if(admin.binding) {
			admin.bind();
		}
		admin.rpc_init(admin.rpc_url);
		
		el = document.getElementById('admin-config-save');
		if(el)
		{
			el.onclick = function(e)
			{
				admin.rpc_config_save();
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

	table_to_json : function(table)
	{ 
		var plugins = {};
		var data = {};
		var plug = [];
		for (var i=1; i<table.rows.length-1; i++) //last row is plugin root path
		{
			var tableRow = table.rows[i];
			var rowData = {};
			var url = [];
			
			var plug_name = tableRow.cells[1].innerHTML;
			if(tableRow.cells[0].children[0].checked)
				plug.push(plug_name);
			
			url.push(tableRow.cells[2].innerHTML);
			url.push("/"+tableRow.cells[2].innerHTML+"/{1}");
			url.push("/"+tableRow.cells[2].innerHTML+"(/(.*))?");
			url.push("1");
			rowData["url"] = url;
			url = [];
			url.push(tableRow.cells[3].innerHTML);
			url.push("/"+tableRow.cells[3].innerHTML+"/{1}");
			url.push("/"+tableRow.cells[3].innerHTML+"(/(.*))?");
			url.push("1");
			rowData["rpc"] = url;
			rowData["skin"] = tableRow.cells[4].innerHTML;
			data[plug_name] = rowData;
		} 
		data["enabled"] = plug;
		data["root"] = table.rows[table.rows.length-1].cells[1].innerHTML;
		plugins["plugins"] = data;
		return plugins;
	},
	
	rpc_init: function(res)
	{
		try {
			admin.RPC = new JsonRpc.ServiceProxy(res, {
				asynchronous: true,  //default value, but if otherwise error raised
				sanitize: false,     //explicit false required, otherwise error raised
				methods: ['set_config'],
				callbackParamName: 'callback'
			});
			JsonRpc.setAsynchronous(admin.RPC, true);
		}
		catch(e){
			alert('Please, add `json-xml-rpc` JavaScript!');
		}
	},
	rpc_config_save: function() {
		var table = admin.table_to_json( document.getElementById('admin-config') );
		//alert(table);
		JsonRpc.setAsynchronous(admin.RPC, true);
		admin.RPC.set_config({params:[ JSON.stringify(table) ],
			onSuccess:function(resultObj)
			{
			},
			onException:function(errorObj)
			{
				document.getElementById('admin-alert').value = errorObj.error;
				//document.getElementById('admin-alert').removeClass("disabled");
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
	}
};

admin.install();