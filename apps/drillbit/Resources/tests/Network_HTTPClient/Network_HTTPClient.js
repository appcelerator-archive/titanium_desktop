describe("Network.HTTPClient",
{
	before: function()
	{
		this.xhr = Titanium.Network.createHTTPClient();
	},
	
	after: function()
	{
		this.xhr = null;
	},
	
	xhr_properties:function()
	{
		value_of(this.xhr).should_be_object();
		
		var methods = ['open','abort','setRequestHeader','send','sendFile',
					   'sendDir','getResponseHeader','setTimeout'];
					
		var props = ['readyState','UNSENT','OPENED','HEADERS_RECEIVED','LOADING',
					 'DONE','responseText','responseXML','status','statusText',
					 'connected','onreadystatechange','ondatastream','onsendstream'];

		for (var c=0;c<methods.length;c++)
		{
			var method = methods[c];
			value_of(this.xhr[method]).should_be_function();
		}
		
		for (var c=0;c<props.length;c++)
		{
			var prop = props[c];
			value_of(this.xhr[prop]).should_not_be_undefined();
		}

		value_of(this.xhr.readyState).should_be(0);
		value_of(this.xhr.connected).should_be_false();
		
		value_of(this.xhr.UNSENT).should_be(0);
		value_of(this.xhr.OPENED).should_be(1);
		value_of(this.xhr.HEADERS_RECEIVED).should_be(2);
		value_of(this.xhr.LOADING).should_be(3);
		value_of(this.xhr.DONE).should_be(4);

	},
	
	twitter_as_async:function(callback)
	{
		value_of(this.xhr).should_be_object();
	
		var timer = null;
		var url = 'http://twitter.com/statuses/public_timeline.json';
		var xhr = this.xhr;
		
		this.xhr.onreadystatechange = function()
		{
			try
			{
				if (this.readyState == xhr.HEADERS_RECEIVED)
				{
					// ah, different web servers it appears on twitter side
					// have to compensate for space and without space separators
					var ct = xhr.getResponseHeader('Content-Type');
					value_of(ct).should_be_string();
					ct = ct.replace(' ','');
					value_of(ct).should_be('application/json;charset=utf-8');
				}
				else if (this.readyState == xhr.DONE)
				{
					clearTimeout(timer);
					var text = xhr.responseText;
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
		this.xhr.open("GET",url);
		value_of(this.xhr.readyState).should_be(this.xhr.OPENED);
		this.xhr.send(null);
		
		timer = setTimeout(function()
		{
			callback.failed('native XHR twitter timed out');
		},20000);
	},
	
	https_test_as_async: function(callback)
	{
		// this is a simple page that can be used (for now) to test
		// HTTPS connectivity
		var url = 'https://api.appcelerator.net/p/v1/app-list';
		var xhr = this.xhr;
		var timer = null;
		
		this.xhr.onreadystatechange = function()
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
		this.xhr.open("GET",url);
		this.xhr.send(null);
		
		timer = setTimeout(function()
		{
			callback.failed('native XHR HTTPS timed out');
		},20000);
	},
	http_test_with_onchange_as_async: function(callback)
	{
		var url = 'http://api.appcelerator.net/p/v1/echo';
		var xhr = this.xhr;
		var timer = null;
		
		this.xhr.onchange = function()
		{
			try
			{
				// if we get here, we connected and received 
				// HTTPS encrypted content
				clearTimeout(timer);
				callback.passed();
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		};
		this.xhr.open("GET",url);
		this.xhr.send(null);
		
		timer = setTimeout(function()
		{
			callback.failed('native XHR HTTP timed out');
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
	}
});
