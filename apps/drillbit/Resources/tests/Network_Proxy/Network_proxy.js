describe("Network.Proxy",{
	// ti.network.HTTP objects are covered in the http_server unit test.

	// test the network object and properties.
	test_network_object:function()
	{
		value_of(Titanium.Network).should_not_be_null();
		value_of(Titanium.Network.getProxy).should_be_function();
		value_of(Titanium.Network.setProxy).should_be_function();

		value_of(Titanium.Network.online).should_be_true();
	},
	
	test_network_Proxy_object: function()
	{
		var theProxy = Titanium.Network.getProxy();
		
		value_of( theProxy ).should_be_object();
		
		value_of( theProxy.getHostName ).should_be_function();
		value_of( theProxy.getPassword ).should_be_function();
		value_of( theProxy.getPort ).should_be_function();
		value_of( theProxy.getUserName ).should_be_function();
		
		this.bNoProxy = false;
	},
	
	test_network_Proxy_object: function()
	{
		var theProxy = Titanium.Network.getProxy();
		
		if ( theProxy )
		{
			value_of( theProxy ).should_be_object();
			
			var host	 = theProxy.getHostName();
			var password = theProxy.getHostName();
			var port	 = theProxy.getHostName();
			var userName = theProxy.getHostName();

			Titanium.API.debug(host);
			Titanium.API.debug(password);
			Titanium.API.debug(port);
			Titanium.API.debug(userName);
		}
		else 
		{
			this.beNoProxy = true;
		}
	},
	
	
	test_network_setProxy: function()
	{
		var host	 = "theHost";
		var password = "thePassword";
		var port	 = "8081";
		var userName = "theUserName";
		
		var bRet = Titanium.Network.setProxy(host, password, port, userName);
		
		value_of(bRet).should_be_true();
		
		if ( this.beNoProxy )
		{
			// if we failed to test the object because we didn't have
			// a proxy configured, then we run the test here.
			var theProxy = Titanium.Network.getProxy();
			value_of( theProxy ).should_be_object();
			
			var host	 = theProxy.getHostName();
			var password = theProxy.getHostName();
			var port	 = theProxy.getHostName();
			var userName = theProxy.getHostName();

			Titanium.API.debug(host);
			Titanium.API.debug(password);
			Titanium.API.debug(port);
			Titanium.API.debug(userName);
		}
		
		if ( bRet )
		{
			// now do we have a proxy?
			if ( Titanium.platform == "win32" )
			{
				// only on windows do we attempt to add an environment variable that 
				// contains the proxy information.  we can use this information to 
				// see if the test passed.
				
				var env = Titanium.API.getEnvironment()["HTTP_PROXY"];
				value_of(env).should_be_string();
			}
		}
	}
});
