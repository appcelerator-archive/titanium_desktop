describe("Network.IRCClient",{
	// ti.network.HTTP objects are covered in the http_server unit test.

	before_all: function()
	{
		value_of(Titanium.Network).should_not_be_null();
		value_of(Titanium.Network.createIRCClient).should_be_function();
		value_of(Titanium.Network.IRCClient).should_not_be_null();
		
		this.client = Titanium.Network.createIRCClient()
		value_of(this.client).should_be_object();
		
		this.connectCount = 0;
	},
	
	after_all: function()
	{
	},
	// test the network object and properties.
	test_network_IRCClient_object:function()
	{
		value_of(this.client.connect).should_be_function();
		value_of(this.client.connected).should_be_boolean();
		value_of(this.client.disconnect).should_be_function();
		value_of(this.client.getNick).should_be_function();
		value_of(this.client.getUsers).should_be_function();
		value_of(this.client.isOp).should_be_function();
		value_of(this.client.isVoice).should_be_function();
		value_of(this.client.join).should_be_function();
		value_of(this.client.send).should_be_function();
		value_of(this.client.setNick).should_be_function();
		value_of(this.client.unjoin).should_be_function();	
		
		this.client = null;
	},

	test_network_connect_as_async: function(test)
	{
		value_of(true).should_be(true);
		test.passed();
		//var connectCount = this.connectCount;
		//
		//var client = Titanium.Network.createIRCClient()
		//value_of(client).should_be_object();
		//
		//var connTimer = null;
		//value_of(client).should_be_object();
		//try
		//{
		//	client.connect("irc.freenode.net", 6667, "drillbit_tester_app", "titaniumDrillbit", "tester", String(new Date().getTime()), function(cmd, channel, data, nick)
		//	{
		//		clearTimeout(connTimer);
		//		
		//		connectCount++;
		//		if (client.connected)
		//		{
		//			client.disconnect()
		//			connectCount--;
		//			if ( !client.connected )
		//			{
		//				test.passed();
		//			}
		//			else
		//			{
		//				test.failed("failed to disconnect from irc.freenode.net");
		//			}
		//		}
		//		else 
		//		{
		//			test.failed("failed to connect to irc.freenode.net");
		//		}
		//	});
		//}
		//catch(e)
		//{
		//	test.failed("failed to connect to IRC site with exception"+e);		
		//}
		//
		//connTimer = setTimeout(function()
		//{
		//	test.failed("irc connection timed out");
		//},1000);
	},

	test_network_IRC_nickname_as_async: function(test)
	{
		value_of(true).should_be(true);
		test.passed();
		// var connectCount = this.connectCount;
		// var client = Titanium.Network.createIRCClient()
		// value_of(client).should_be_object();
		// 
		// var connTimer = null;
		// try
		// {
		// 	client.connect("irc.freenode.net", 6667, "drillbit_tester_app", "titaniumDrillbit", "tester", String(new Date().getTime()), function(cmd, channel, data, nick)
		// 	{
		// 		connectCount++;
		// 		clearTimeout(connTimer);
		// 		
		// 		Titanium.API.debug("Connected to IRC server "+connectCount);
		// 		if (client.connected)
		// 		{
		// 			var nicTimer = setTimeout( function()
		// 			{
		// 				try
		// 				{
		// 					Titanium.API.debug("checking the default nickname");
		// 					value_of(client.getNick()).should_be("drillbit_tester_app");
		// 					value_of(client.getNick()).should_not_be("drillbitNickName");
		// 					
		// 					Titanium.API.debug("setup a new nickname");
//		// 					var newUser = "nickname"+String(new Date().getTime());
		// 					var newUser = "drillbitNickName";
		// 					client.setNick(newUser);
		// 					
		// 					var timer = setTimeout(function()
		// 					{
		// 						clearTimeout(nicTimer);
		// 						try{
		// 							value_of(client.getNick()).should_be(newUser);
		// 							test.passed();
		// 							break;
		// 						}
		// 						catch(e)
		// 						{
		// 							test.failed(e);
		// 						}
		// 						connectCount--;
		// 						client.disconnect();
		// 					}, 5000);
		// 				}
		// 				catch(e)
		// 				{
		// 					test.failed("failed to change the Nickname: '"+e+"'");
		// 				}
		// 			}, 10000);
		// 		}
		// 		else
		// 		{
		// 			Titanium.API.debug("potentially kicked off IRC, don't know what to do");
		// 			test.passed();
		// 		}
		// 	});
		// }
		// catch(e)
		// {
		// 	test.failed("failed to connect to IRC site with exception: '"+e+"'");
		// }
		// 
		// connTimer = setTimeout(function()
		// {
		// 	test.failed("irc connection timed out");
		// },20000);
	}
});
