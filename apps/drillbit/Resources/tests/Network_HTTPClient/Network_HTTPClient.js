describe("Network.HTTPClient",
{
	before_all: function()
	{
		// Launch test http server
		this.httpd = Titanium.Process.createProcess(
		{
			args: [
				'python', Titanium.API.application.resourcesPath + "/httpd.py"
			],
		});
		this.httpd.launch();
	},

	after_all: function()
	{
		this.httpd.kill();
	},

	before: function()
	{
		this.client = Titanium.Network.createHTTPClient();
	},
	
	after: function()
	{
		this.client = null;
	},
	
	client_properties:function()
	{
		value_of(this.client).should_be_object();
		
		var methods = ['open','abort','setRequestHeader','send','sendFile',
					   'sendDir','getResponseHeader','setTimeout'];
					
		var props = ['readyState','UNSENT','OPENED','HEADERS_RECEIVED','LOADING',
					 'DONE','responseText','responseXML','status','statusText',
					 'connected','onreadystatechange','ondatastream','onsendstream', 'onload'];

		for (var c=0;c<methods.length;c++)
		{
			var method = methods[c];
			value_of(this.client[method]).should_be_function();
		}
		
		for (var c=0;c<props.length;c++)
		{
			var prop = props[c];
			value_of(this.client[prop]).should_not_be_undefined();
		}

		value_of(this.client.readyState).should_be(0);
		value_of(this.client.connected).should_be_false();
		
		value_of(this.client.UNSENT).should_be(0);
		value_of(this.client.OPENED).should_be(1);
		value_of(this.client.HEADERS_RECEIVED).should_be(2);
		value_of(this.client.LOADING).should_be(3);
		value_of(this.client.DONE).should_be(4);

	},
	
	twitter_as_async:function(callback)
	{
		value_of(this.client).should_be_object();
	
		var timer = null;
		var url = 'http://twitter.com/statuses/public_timeline.json';
		var client = this.client;
		
		this.client.onreadystatechange = function()
		{
			try
			{
				if (this.readyState == client.HEADERS_RECEIVED)
				{
					// ah, different web servers it appears on twitter side
					// have to compensate for space and without space separators
					var ct = client.getResponseHeader('Content-Type');
					value_of(ct).should_be_string();
					ct = ct.replace(' ','');
					value_of(ct).should_be('application/json;charset=utf-8');
				}
				else if (this.readyState == client.DONE)
				{
					clearTimeout(timer);
					var text = client.responseText;
					value_of(text).should_be_string();
					value_of(text.length > 0).should_be_true();
					callback.passed();
				}
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		};
		this.client.open("GET",url);
		value_of(this.client.readyState).should_be(this.client.OPENED);
		this.client.send(null);
		
		timer = setTimeout(function()
		{
			callback.failed('twitter test timed out');
		},20000);
	},
	
	https_test_as_async: function(callback)
	{
		// this is a simple page that can be used (for now) to test
		// HTTPS connectivity
		var url = 'https://api.appcelerator.net/p/v1/app-list';
		var client = this.client;
		var timer = null;
		
		this.client.onreadystatechange = function()
		{
			try
			{
				if (this.readyState == this.DONE)
				{
					// if we get here, we connected and received 
					// HTTPS encrypted content
					clearTimeout(timer);
					callback.passed();
				}
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		};
		this.client.open("GET",url);
		this.client.send(null);
		
		timer = setTimeout(function()
		{
			callback.failed('HTTPS test timed out');
		},20000);
	},

	test_encode_decode: function()
	{
		var foo = Titanium.Network.encodeURIComponent(null);
		value_of(foo).should_be('');
		
		foo = Titanium.Network.encodeURIComponent('');
		value_of(foo).should_be('');
		
		foo = Titanium.Network.encodeURIComponent('a');
		value_of(foo).should_be('a');

		foo = Titanium.Network.decodeURIComponent(null);
		value_of(foo).should_be('');

		foo = Titanium.Network.decodeURIComponent('');
		value_of(foo).should_be('');

		foo = Titanium.Network.decodeURIComponent('a');
		value_of(foo).should_be('a');
		
		foo = Titanium.Network.encodeURIComponent('a b');
		value_of(foo).should_be('a%20b');

		foo = Titanium.Network.decodeURIComponent(foo);
		value_of(foo).should_be('a b');
	},

	http_onload_as_async: function(callback)
	{
		var url = 'http://127.0.0.1:8888';
		var client = this.client;
		var timer = null;
		
		this.client.onload = function()
		{
			try
			{
				var response = client.responseText;
				value_of(response).should_be("here is some text for you!");
			
				clearTimeout(timer);
				callback.passed();
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		};
		this.client.open("GET",url);
		this.client.send(null);
		
		timer = setTimeout(function()
		{
			callback.failed('native XHR HTTP timed out');
		},20000);
	},

	test_ondatastream_as_async: function(callback)
	{
		var client = Titanium.Network.createHTTPClient();
		var timer = 0;
		client.ondatastream = function() {
			clearTimeout(timer);
			callback.passed();
		
			/*try
			{
				//totalCount, totalSize, buffer, length
				clearTimeout(timer);
				value_of(list[3]).should_be_greater_than(0);
				value_of(buffer.length).should_be(list[3]);
				callback.passed();
			}
			catch (e)
			{
				callback.failed(e);
			}*/
		};
		
		timer = setTimeout(function()
		{
			callback.failed('ondatastream timed out');
		},3000);
		
		client.open("GET", "http://api.appcelerator.net/p/v1/echo");
		client.send(null);
	}
});
