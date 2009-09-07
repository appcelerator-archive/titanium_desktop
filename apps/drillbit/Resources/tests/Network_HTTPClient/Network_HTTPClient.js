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

		this.text = "here is some text for you!";
	},

	after_all: function()
	{
		this.httpd.kill();
	},

	before: function()
	{
		this.client = Titanium.Network.createHTTPClient();
		this.url = "http://127.0.0.1:8888/";
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
		},5000);
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

	http_onreadystate_as_async: function(callback)
	{
		var timer = null;

		this.client.onreadystatechange = function()
		{
			try
			{
				value_of(this.readyState).should_be_number();
				if (this.readyState == this.DONE)
				{
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

		timer = setTimeout(function()
		{
			callback.failed('HTTP onreadystate test timed out');
		},5000);

		this.client.open("GET", this.url);
		this.client.send(null);
	},

	test_ondatastream_as_async: function(callback)
	{
		var timer = null;
		var text = this.text;
		var readCount = 0;

		this.client.ondatastream = function(totalRead, totalSize, buffer, bufferLength)
		{	
			try
			{
				value_of(this).should_be_object();
				value_of(totalRead).should_be_number();
				value_of(totalSize).should_be_number();
				value_of(buffer).should_be_object();
				value_of(bufferLength).should_be_number();

				value_of(totalRead).should_be(readCount + buffer.length);
				value_of(totalSize).should_be(text.length);
				value_of(buffer).should_be(text.substring(readCount, readCount+buffer.length));
				value_of(bufferLength).should_be(buffer.length);

				readCount += buffer.length;
				if (readCount >= text.length)
				{
					clearTimeout(timer);
					callback.passed();
				}
			}
			catch (e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		};
		
		timer = setTimeout(function()
		{
			callback.failed('HTTP ondatastream test timed out');
		},5000);
		
		this.client.open("GET", this.url);
		this.client.send(null);
	},

	/*http_onsendstream_as_async: function(callback)
	{
		var timer = null;
		var text = this.text;

		this.client.onsendstream = function()
		{
		},
	},*/

	http_onload_as_async: function(callback)
	{
		var text = this.text;
		var timer = null;
		
		this.client.onload = function()
		{
			try
			{
				value_of(this.responseText).should_be(text);
			
				clearTimeout(timer);
				callback.passed();
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		};
			
		timer = setTimeout(function()
		{
			callback.failed('HTTP onload test timed out');
		},5000);

		this.client.open("GET", this.url);
		this.client.send(null);
	},
});
