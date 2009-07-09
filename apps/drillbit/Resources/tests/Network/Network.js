describe("Network Module",{
	// ti.network.HTTP objects are covered in the http_server unit test.

	// test the network object and properties.
	test_network_object:function()
	{
		value_of(Titanium.Network).should_not_be_null();
		value_of(Titanium.Network.getProxy).should_be_function();
		value_of(Titanium.Network.setProxy).should_be_function();
		value_of(Titanium.Network.addConnectivityListener).should_be_function();
		value_of(Titanium.Network.createHTTPClient).should_be_function();
		value_of(Titanium.Network.createHTTPServer).should_be_function();
		value_of(Titanium.Network.createIPAddress).should_be_function();
		value_of(Titanium.Network.createIRCClient).should_be_function();
		value_of(Titanium.Network.createTCPSocket).should_be_function();
		value_of(Titanium.Network.decodeURIComponent).should_be_function();
		value_of(Titanium.Network.encodeURIComponent).should_be_function();
		value_of(Titanium.Network.getHostByAddress).should_be_function();
		value_of(Titanium.Network.getHostByName).should_be_function();
		value_of(Titanium.Network.online).should_not_be_null();
		value_of(Titanium.Network.removeConnectivityListener).should_be_function();
	 
		value_of(Titanium.Network.online).should_be_true();
	},
	
	test_network_URIComponents: function()
	{
		var str = "the test string";
		
		var encoded = Titanium.Network.encodeURIComponent(str);
		
		// is the string encoded?
		value_of(str == encoded).should_be_false();
		value_of(encoded.indexOf("%20")).should_not_be(-1);
		
		var decoded = Titanium.Network.decodeURIComponent(encoded);

		// is the string decoded?
		value_of(str == decoded).should_be_true();
		value_of(decoded.indexOf("%20")).should_be(-1);
	}
});
