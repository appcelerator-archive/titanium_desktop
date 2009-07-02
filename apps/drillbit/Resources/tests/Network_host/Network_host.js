describe("Network module tests",{
    // test the network object and properties.
	test_network_object:function()
	{
	    value_of(Titanium.Network).should_not_be_null();
        value_of(Titanium.Network.getHostByAddress).should_be_function();
        value_of(Titanium.Network.getHostByName).should_be_function();
	},
	
    test_network_Host_object: function()
	{
	    var theHost = Titanium.Network.getHostByName("127.0.0.1");
	    
	    value_of(theHost.getAddresses).should_be_function();
	    value_of(theHost.getAliases).should_be_function();
	    value_of(theHost.getName).should_be_function();
	    value_of(theHost.isInvalid).should_be_function();
	    value_of(theHost.toString).should_be_function();
	},

	test_network_Host_by_address: function()
	{
	    var theHost = Titanium.Network.getHostByName("127.0.0.1");
	    value_of(theHost).should_be_object();
	    value_of(theHost.isInvalid()).should_be_false();

	    value_of(theHost.getName()).should_be("127.0.0.1");
        Titanium.API.debug(theHost.toString());
	},
	
	test_network_Host_by_name: function()
	{
	    var theHost = Titanium.Network.getHostByName("localhost");
	    value_of(theHost).should_be_object();
	    value_of(theHost.isInvalid()).should_be_false();

        // this should be the machine name.
	    value_of(theHost.getName()).should_not_be("localhost");
        Titanium.API.debug(theHost.toString());
	},
	
    test_network_Host_addresses: function()
	{
	    var theHost = Titanium.Network.getHostByName("127.0.0.1");
	    value_of(theHost).should_be_object();
	    value_of(theHost.isInvalid()).should_be_false();
	   
	    value_of(theHost.getAddresses).should_be_function();
	    
	    var alist = theHost.getAddresses();
	    value_of(alist).should_be_array();
	},
	
    test_network_Host_aliases: function()
	{
	    var theHost = Titanium.Network.getHostByName("127.0.0.1");
	    value_of(theHost).should_be_object();
	    value_of(theHost.isInvalid()).should_be_false();

	    value_of(theHost.getAliases).should_be_function();

	    var alist = theHost.getAliases();
	    value_of(alist).should_be_array();
	}	
});