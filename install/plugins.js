{
	"plugins" : {
		"root" : "plugins",
		"enabled" : [ "admin" , "user" ],

                "admin" : {
			"skin" : "admin_view",
                        "url": ["admin", "/admin/{1}", "/admin/(/(.*))?", "1" ],
                        "rpc": ["admin_rpc", "/admin_rpc/{1}", "/admin_rpc(/(.*))?" , "1" ]
                },

		"wiki" : {
			"url": ["wiki", "/w/{1}", "/w((/.*)?)", "1" ],
			"rpc": ["wiki_rpc", "/w_rpc/{1}", "/w_rpc((/.*)?)" , "1" ]
                },

		"user" : {
			"url": ["user", "/user/{1}", "/user(/(.*))?)", "1" ],
                        "rpc": ["user_rpc", "/user_rpc/{1}", "/user_rpc(/(.*))?" , "1" ]
		}
	}
}

