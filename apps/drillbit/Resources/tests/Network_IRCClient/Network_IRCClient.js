describe("Network IRCClient object tests",{
    // ti.network.HTTP objects are covered in the http_server unit test.

    before_all: function()
    {
	    value_of(Titanium.Network).should_not_be_null();
        value_of(Titanium.Network.createIRCClient).should_be_function();
        value_of(Titanium.Network.IRCClient).should_not_be_null();
        
	    this.client = Titanium.Network.createIRCClient()
	    value_of(this.client).should_be_object();
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
	},

	test_network_connect_as_async: function(test)
	{
        var self = this;
	    var connTimer = null;
	    value_of(this.client).should_be_object();
	    try
	    {
            self.client.connect("irc.freenode.net", 6667, "drillbit_tester_app", "titaniumDrillbit", "tester", String(new Date().getTime()), function(cmd, channel, data, nick)
            {
                clearTimeout(connTimer);
                
                if (self.client.connected)
                {
                    self.client.disconnect()
                    if ( !self.client.connected )
                    {
                        test.passed();
                    }
                    else
                    {
                        test.failed("failed to disconnect from irc.freenode.net");
                    }
                }
                else 
                {
                    test.failed("failed to connect to irc.freenode.net");
                }
            });
	    }
	    catch(e)
	    {
            test.failed("failed to connect to IRC site with exception"+e);	    
	    }
	    
	    connTimer = setTimeout(function()
	    {
	        test.failed("irc connection timed out");
	    },1000);
	}
	
	/*
	    this test always fails with a system excpetion.  we had it working a little, but it'
	    unreliable.  Lots of network timing issues to sort out.
	,

	test_network_IRC_nickname_as_async: function(test)
	{
        var self = this;
	    var connTimer = null;
	    value_of(this.client).should_be_object();
	    try
	    {
            self.client.connect("irc.freenode.net", 6667, "drillbit_tester_app_test2", "titaniumDrillbit1", "tester1", String(new Date().getTime()), function(cmd, channel, data, nick)
            {
                clearTimeout(connTimer);
                var nicTimer = setTimeout( function()
                {
                    var newUser = "nickname"+String(new Date().getTime());
                    value_of(self.client.getNick()).should_be("drillbit_tester_app");
                    self.client.setNick(newUser);
                    
                    var timer = setTimeout(function()
                    {
                        clearTimeout(nicTimer);
                        try{
                            value_of(self.client.getNick()).should_be(newUser);
                            test.passed();
                        }
                        catch(e)
                        {
                            test.failed(e);
                        }
                        self.client.disconnect();
                    }, 5000);
                }, 10000);
            });
	    }
	    catch(e)
	    {
            test.failed("failed to connect to IRC site with exception"+e);	    
	    }
	    
	    connTimer = setTimeout(function()
	    {
	        test.failed("irc connection timed out");
	    },20000);
	}	
	*/
});