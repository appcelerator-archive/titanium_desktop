describe("Network.Proxy",{
	before_all: function()
	{
		this.hasProxy = false;
	},
	
	test_network_object:function()
	{
		value_of(Titanium.Network).should_not_be_null();
		value_of(Titanium.Network.getProxy).should_be_function();
		value_of(Titanium.Network.setProxy).should_be_function();
		value_of(Titanium.Network.online).should_be_true();
	},
	
	test_proxy: function()
	{
		var proxy = Titanium.Network.getProxy();
		if (proxy)
		{
			value_of(proxy).should_be_object();
			value_of(proxy.getHostName()).should_not_be_undefined();
			value_of(proxy.getPort()).should_be_number();
		}
		else 
		{
			this.hasProxy = true;
		}
	},
	
	test_network_setProxy: function()
	{
		var host = "host";
		var password = "password";
		var port = "8081";
		var username = "username";
		
		var succesful = Titanium.Network.setProxy(host, password, port, username);
		value_of(succesful).should_be_true();
		
		if (this.hasProxy)
		{
			// if we failed to test the object because we didn't have
			// a proxy configured, then we run the test here.
			var proxy = Titanium.Network.getProxy();
			value_of(proxy).should_be_object();
			value_of(proxy.getHostName()).should_not_be_undefined();
			value_of(proxy.getPort()).should_be_number();
		}
		
		if (succesful)
		{
			// now do we have a proxy?
			if (Titanium.platform == "win32")
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
