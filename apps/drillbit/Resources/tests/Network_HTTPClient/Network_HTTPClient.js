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
		this.reply = "I got it!";
		this.filepath = Titanium.API.application.resourcesPath + "/test.txt";
	},

	after_all: function()
	{
		this.httpd.kill();
	},

	before: function()
	{
		this.url = "http://127.0.0.1:8888/";
		this.client = Titanium.Network.createHTTPClient();
	},
	
	after: function()
	{
		this.client = null;
	},

	test_client_properties:function()
	{
		value_of(this.client).should_be_object();
		
		var methods = ['open','abort','setRequestHeader','send','sendFile',
					   'getResponseHeader','setTimeout'];
					
		var props = ['readyState','UNSENT','OPENED','HEADERS_RECEIVED','LOADING',
					 'DONE','responseText','responseXML','status','statusText',
					 'connected', 'timedOut'];

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
		value_of(this.client.timedOut).should_be_false();
		
		value_of(this.client.UNSENT).should_be(0);
		value_of(this.client.OPENED).should_be(1);
		value_of(this.client.HEADERS_RECEIVED).should_be(2);
		value_of(this.client.LOADING).should_be(3);
		value_of(this.client.DONE).should_be(4);

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

	test_open: function()
	{
		value_of(this.client.open("GET", "http://test.com")).should_be_true();
		value_of(this.client.open("GET", "https://test.com")).should_be_true();
		value_of(function() { this.client.open("GET", "bad://test.com")}).should_throw_exception();
	},

	test_async_get_as_async: function(callback)
	{
		var timer = null;
		var text = this.text;
		var client = this.client;
		var url = this.url;
		// We want to make sure the web server has started up, so sleep
		// a little bit before starting this test.
		function runTest()
		{
			client.addEventListener(Titanium.HTTP_DONE, function()
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
			});

			timer = setTimeout(function()
			{
				callback.failed('Async GET test timed out');
			}, 10000);

			client.open("GET", url);
			client.send(null);
		}

		setTimeout(runTest, 1000);
	},


	test_sync_get: function()
	{
		done = false;

		this.client.addEventListener(Titanium.HTTP_DONE, function()
		{
			done = true;
		});

		this.client.open("GET", this.url, false);
		value_of(this.client.send(null)).should_be_true();
		value_of(done).should_be_true();
		value_of(this.client.responseText).should_be(this.text);
	},

	test_timeout_as_async: function(callback)
	{
		var timer = null;

		this.client.addEventListener(Titanium.HTTP_TIMEOUT, function(e)
		{
			try
			{
				value_of(this.timedOut).should_be_true();

				clearTimeout(timer);
				callback.passed();
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		});

		timer = setTimeout(function()
		{
			// We should timeout only after 1 second
			callback.failed('Timeout test did not time out');
		},10000);

		this.client.setTimeout(1000);
		this.client.open("GET", this.url + "longrequest");
		this.client.send(null);
	},

	test_abort_as_async: function(callback)
	{
		var timer = null;

		this.client.addEventListener(Titanium.HTTP_ABORT, function()
		{
			clearTimeout(timer);
			callback.passed();
		});

		this.client.addEventListener(Titanium.HTTP_DONE, function()
		{
			// This should not get called
			clearTimeout(timer);
			callback.failed("Request was not aborted");
		});

		timer = setTimeout(function()
		{
			// Abort
			callback.failed("Abort test timed out");
		},10000);

		this.client.open("GET", this.url + "longrequest");
		this.client.send(null);
		this.client.abort();
	},

	test_https_as_async: function(callback)
	{
		// this is a simple page that can be used (for now) to test
		// HTTPS connectivity
		var url = 'https://api.appcelerator.net/p/v1/app-list';
		var client = this.client;
		var timer = 0;

		this.client.addEventListener(Titanium.HTTP_DONE, function()
		{
			clearTimeout(timer);

			if (client.status != 200)
			{
				callback.failed("Status should have been 200, but was: " + client.status);
			}
			else
			{
				callback.passed();
			}
		});

		timer = setTimeout(function()
		{
			callback.failed('HTTPS test timed out');
		},10000);

		this.client.open("GET", url);
		this.client.send(null);
	},

	test_post_as_async: function(callback)
	{
		var timer = 0;
		var reply = this.reply;

		this.client.addEventListener(Titanium.HTTP_DONE, function(e)
		{
			try
			{
				value_of(this.status).should_be(200);
				value_of(this.responseText).should_be(reply);

				clearTimeout(timer);
				callback.passed();
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		});

		timer = setTimeout(function()
		{
			callback.failed('POST test timed out');
		},10000);

		this.client.open("POST", this.url + "recvpostdata");
		this.client.send(this.text);
	},

	test_post_paramters_as_async: function(callback)
	{
		var timer = 0;
		var reply = this.reply;

		this.client.addEventListener(Titanium.HTTP_DONE, function(e)
		{
			try
			{
				value_of(this.status).should_be(200);
				value_of(this.responseText).should_be(reply);
				clearTimeout(timer);
				callback.passed();
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		});

		timer = setTimeout(function()
		{
			callback.failed('POST test timed out');
		},10000);

		this.client.open("POST", this.url + "recvpostparams");
		this.client.send({
			'one': 'flippityflop',
			'two': 'bloopityblop',
			'three': ''
		})
	},


	test_redirect_as_async: function(callback)
	{
		var timer = 0;
		var text = this.text;
		var url = this.url;

		this.client.addEventListener(Titanium.HTTP_REDIRECT, function()
		{
			try
			{
				value_of(this.url).should_be(url);
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		});
		this.client.addEventListener(Titanium.HTTP_DONE, function()
		{
			try
			{
				value_of(this.status).should_be(200);
				value_of(this.responseText).should_be(text);

				clearTimeout(timer);
				callback.passed();
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		});

		timer = setTimeout(function()
		{
			callback.failed('Redirect test timed out');
		},10000);

		this.client.open("GET", this.url + "301redirect");
		this.client.send(null);
	},

	test_receive_text_as_async: function(callback)
	{
		var timer = 0;
		var text = this.text;
		var data = "";

		this.client.addEventListener(Titanium.HTTP_DONE, function()
		{
			try
			{
				value_of(data).should_be(text);

				clearTimeout(timer);
				callback.passed();
			}
			catch(e)
			{
				clearTimeout(timer);
				callback.failed(e);
			}
		});

		timer = setTimeout(function()
		{
			callback.failed('Receive text test timed out');
		},10000);

		this.client.open("GET", this.url);
		this.client.receive(function (payload)
		{
			data += payload;
		});
	},

	test_send_cookie: function()
	{
		this.client.setCookie("peanutbutter", "yummy");
		this.client.open("GET", this.url + "sendcookie", false);
		this.client.send(null);
		value_of(this.client.status).should_be("200");
		value_of(this.client.responseText).should_be("got the cookie!");
	},

	test_recv_cookie: function()
	{
		this.client.open("GET", this.url + "recvcookie", false);
		this.client.send(null);
		value_of(this.client.status).should_be("200");
		var cookie = this.client.getCookie("chocolatechip");
		value_of(cookie).should_be_object();
		value_of(cookie.value).should_be("tasty")
	},

	test_basic_auth: function()
	{
		this.client.setBasicCredentials("test", "password");
		this.client.open("GET", this.url + "basicauth", false);
		this.client.send(null);
		value_of(this.client.status).should_be("200");
		value_of(this.client.responseText).should_be("authorized");
	},

	test_send_file: function()
	{
		var file = Titanium.Filesystem.getFile(this.filepath);
		value_of(file).should_be_object();
		this.client.open("POST", this.url + "recvfile", false);
		this.client.send(file);
		value_of(this.client.status).should_be("200");
		value_of(this.client.responseText).should_be("Got the file!");
	},

	test_handlers_as_async: function(callback)
	{
		var text = this.text;

		var onreadystatechange = false;
		var ondatastream = false;

		this.client.onreadystatechange = function()
		{
			try
			{
				onreadystatechange = true;
				value_of(this.readyState).should_be_number();
				value_of(this.readyState >= 1 && this.readyState <= 4).should_be_true();
			}
			catch(e)
			{
				callback.failed(e);
			}
		}
		this.client.ondatastream = function()
		{
			try
			{
				ondatastream = true;
				value_of(this.dataReceived).should_be(text.length);
			}
			catch(e)
			{
				callback.failed(e);
			}
		}
		this.client.onload = function()
		{
			try
			{
				onload = true;
				value_of(this.readyState).should_be("4");
				value_of(this.status).should_be("200");
				value_of(this.responseText).should_be(text);

				value_of(onreadystatechange).should_be_true();
				value_of(ondatastream).should_be_true();

				callback.passed();
			}
			catch(e)
			{
				callback.failed(e);
			}
		}

		this.client.open("GET", this.url);
		this.client.send();
	},

	test_onload_as_async: function(callback)
	{
		// Test onload handler when an onreadystatechange handler is not installed
		// See bug #335
		this.client.onload = function()
		{
			callback.passed()
		}

		this.client.open("GET", this.url, false);
		this.client.send();
		timer = setTimeout(function()
		{
			callback.failed('Test timed out');
		}, 10000);
	},

	test_request_headers: function()
	{
		this.client.setRequestHeader("Foo", "Bar");
		this.client.setRequestHeader("Hello", "World");
		this.client.setRequestHeader("Head", "Tail");
		this.client.open("GET", this.url + "requestheaders", false);
		this.client.send();
		value_of(this.client.status).should_be("200");
		value_of(this.client.responseText).should_be("Got the headers!");
	},

	test_get_response_header: function()
	{
		this.client.open("GET", this.url + "responseheaders", false);
		this.client.send();
		value_of(this.client.status).should_be("200");
		value_of(this.client.getResponseHeader("Foo")).should_be("Bar");
		value_of(this.client.getResponseHeader("Head")).should_be("Tail");
	},

	test_get_response_headers: function()
	{
		this.client.open("GET", this.url + "responseheaders", false);
		this.client.send();
		value_of(this.client.status).should_be("200");

		var headers = this.client.getResponseHeaders();
		value_of(headers).should_be_array();
		var sawFoo = false;
		var sawHead = false;

		for (var i = 0; i < headers.length; i++)
		{
			var h = headers[i]
			if (h[0] == "Foo")
			{
				sawFoo = true;
				value_of(h[1]).should_be("Bar");
			}
			if (h[0] == "Head")
			{
				sawHead = true;
				value_of(h[1]).should_be("Tail");
			}
		}

		//value_of(sawFoo).should_be_true();
		//value_of(sawHead).should_be_true();
	},
	test_http_continue_as_async: function(callback)
	{
		// Test onload handler when an onreadystatechange handler is not installed
		// See bug #335
		this.client.onload = function()
		{
			if (this.status != 200)
				callback.failed("status should be 200, but was: " + this.status);

			callback.passed()
		}

		this.client.open("GET", this.url + "continue", false);
		this.client.send();
		timer = setTimeout(function()
		{
			callback.failed('Test timed out');
		}, 10000);
	},
	test_default_user_agent_as_async: function(callback)
	{
		// google.fr will not send utf8 data unless we give it a reasonable
		// user agent (event accept-charset has no affect), so just make sure
		// that the output properly converted to UTF8. Bug #353.
		this.client.onload = function()
		{
			if (this.responseText.length < 10) {
				callback.failed("responseText wasn't large enough");
			} else {
				callback.passed()
			}
		}

		this.client.open("GET", "http://www.google.fr");
		this.client.send();
		timer = setTimeout(function()
		{
			callback.failed('Test timed out');
		}, 10000);
	},

});
