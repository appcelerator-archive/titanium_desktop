describe("Network.TCPSocket",{
	// ti.network.HTTP objects are covered in the http_server unit test.

	before_all: function()
	{
		value_of(Titanium.Network).should_not_be_null();
		value_of(Titanium.Network.createTCPSocket).should_be_function();
		value_of(Titanium.Network.TCPSocket).should_not_be_null();
		
		// create a socket to the localhost web server.
		// this test assumes you have a webserver running locally (apache, IIS, whatever)
		this.socket = Titanium.Network.createTCPSocket("127.0.0.1", 80);
		
		// to test the socket read and write functionality we setup a 
		// temporary HTTP_SERVER object to open a connection to.
		this.myHTTPServer = Titanium.Network.createHTTPServer();
		
		this.myHTTPServer.bind(8082,function(request,response)
		{
			try
			{
				value_of(request.getMethod()).should_be('GET');
				value_of(request.getURI()).should_be('/foo');
				response.setContentType('text/plain');
				response.setContentLength(3);
				response.setStatusAndReason('200','OK');
				response.write('123');
			}
			catch(e)
			{
				this.myHTTPServer = null;
			}
		});
	},
	
	after_all: function()
	{
		this.socket = null;
		this.myHTTPServer = null;	
	},
	// test the network object and properties.
	test_network_TCPSocket_object:function()
	{
		value_of(this.socket).should_be_object();
		
		value_of(this.socket.close).should_be_function();
		value_of(this.socket.connect).should_be_function();
		value_of(this.socket.isClosed).should_be_function();
		value_of(this.socket.onRead).should_be_function();
		value_of(this.socket.onReadComplete).should_be_function();
		value_of(this.socket.onTimeout).should_be_function();
		value_of(this.socket.onWrite).should_be_function();
		value_of(this.socket.write).should_be_function();
	},
	
	test_network_TCPSocket_isClosed: function()
	{
		value_of(this.socket.isClosed).should_be_function();
		value_of(this.socket.isClosed()).should_be_true();
	},
	
	test_network_TCPSocket_connect_as_async: function(test)
	{
		var mysocket = this.socket;
		value_of(this.socket.connect).should_be_function();
		value_of(this.socket.close).should_be_function();
		value_of(this.socket.isClosed()).should_be_true();
		value_of(this.socket.connect()).should_be_true();

		setTimeout(function() {
			// Connect happens asynchronously now, so isClosed might not 
			// be true immediately.
			try {
				value_of(mysocket.isClosed()).should_be_false();
				value_of(mysocket.close()).should_be_true();
				value_of(mysocket.isClosed()).should_be_true();
				test.passed();
			} catch (exception) {
				test.failed(String(exception));
			}
		}, 1000);
	},
	
	test_TCPSocket_write_as_async: function(test)
	{
		var bresult = false;
		var timer = null;
		// create a socket to the appcelerator web server.
		var TCPsocket = Titanium.Network.createTCPSocket("127.0.0.1", 8082);

		TCPsocket.onTimeout( function(buf)
		{
			clearTimeout(timer);
			test.failed("socket timed out");
		});
		
		try 
		{
			bresult = TCPsocket.connect();		
		}
		catch(e)
		{
			// during testing I found that this exception 
			// will never get hit because we don't intercept the 
			// exception here, only drillbit seems capable enough
			// to handle this.  kind of sucks since I could use this as a test.
			test.failed("failed with exception on connect"+e);
		}
			
		if ( bresult )
		{
			try
			{
				// we opened the socet to the localhost website.
				// we should get something back regaurdless.
				bresult = TCPsocket.write("GET /foo");
				if ( !bresult )
				{
					test.failed("failed to write data to TCPsocket"+this.TCPsocket);
				}
				test.passed();
			}
			catch(e)
			{
				test.failed("TCPSocket error: "+e);			
			}
			TCPsocket.close();
		}
		else 
		{
			test.failed("socket is not connected");
		}
		
		// fail test after 2s
		timer = setTimeout(function()
		{
			test.failed('TCPsocket read/write test timed out after 2 seconds');
		},2000);
	},
	
	test_TCPSocket_onRead_as_async: function(test)
	{
		// create a socket to the appcelerator web server.
		var TCPsocket = null;
		var TCPsocket = Titanium.Network.createTCPSocket("127.0.0.1", 8082);

		var output = null;
		var timer = null;
		TCPsocket.onRead(function(buf)
		{
			if (buf == null)
			{
				test.failed("buffer sent to onRead is null");
			}
			else 
			{
				output += buf;
				test.passed();
				clearTimeout(timer);
			}
		});
		
		TCPsocket.onTimeout( function(buf)
		{
			clearTimeout(timer);
			test.failed("socket timed out");
		});
		
		var bresult = false;
		
		try 
		{
			bresult = TCPsocket.connect();		
		}
		catch(e)
		{
			// during testing I found that this exception 
			// will never get hit because we don't intercept the 
			// exception here, only drillbit seems capable enough
			// to handle this.  kind of sucks since I could use this as a test.
			test.failed("failed with exception on connect"+e);
		}
			
		if ( bresult )
		{
			try
			{
				// we opened the socet to the localhost website.
				// we should get something back regaurdless.
				bresult = TCPsocket.write("GET /foo");
				if ( !bresult )
				{
					test.failed("failed to write data to TCPsocket"+this.TCPsocket);
				}
				test.passed();
			}
			catch(e)
			{
				test.failed("TCPSocket error: "+e);
			}
			TCPsocket.close();
		}
		else 
		{
			test.failed("socket is not connected");
		}
		
		// fail test after 2s
		timer = setTimeout(function()
		{
			if ( output.length <= 0 )
			{
				test.failed('TCPsocket read/write test timed out after 2 seconds');
			}
			else
			{
				test.passed();
			}
		},2000);
	},

	test_TCPSocket_as_async: function(test)
	{
		// create a socket to the appcelerator web server.
		var TCPsocket = null;
		var TCPsocket = Titanium.Network.createTCPSocket("127.0.0.1", 8082);

		var output = null;
		var timer = null;
		TCPsocket.onRead( function(buf)
		{
			if ( buf == null )
			{
				test.failed("buffer sent to onRead is null");
			}
			else 
			{
				output += buf;
			}
		});
		
		TCPsocket.onReadComplete( function(buf)
		{
			clearTimeout(timer);
			try
			{
				if ( output.length > 0 )
				{
					// do something mindless to try and chuck an exception
					Titanium.API.debug(output);
					test.passed();
				}
				else 
				{
					// failed to read any data  (onRead callback not called)
					test.failed("output buffer empty");
				}
			}
			catch(e)
			{
				// problems with the output, fail...
				test.failed("onReadComplete threw an exception, output buffer is null");
			}
		});
		
		TCPsocket.onTimeout( function(buf)
		{
			clearTimeout(timer);
			test.failed("socket timed out");
		});
		
		var bresult = false;
		
		try 
		{
			bresult = TCPsocket.connect();		
		}
		catch(e)
		{
			// during testing I found that this exception 
			// will never get hit because we don't intercept the 
			// exception here, only drillbit seems capable enough
			// to handle this.  kind of sucks since I could use this as a test.
			test.failed("failed with exception on connect"+e);
		}
			
		if ( bresult )
		{
			try
			{
				// we opened the socet to the localhost website.
				// we should get something back regaurdless.
				bresult = TCPsocket.write("GET /foo");
				if ( !bresult )
				{
					test.failed("failed to write data to TCPsocket"+this.TCPsocket);
				}
				test.passed();
			}
			catch(e)
			{
				test.failed("TCPSocket error: "+e);			
			}
			TCPsocket.close();
		}
		else 
		{
			test.failed("socket is not connected");
		}
		
		// fail test after 2s
		timer = setTimeout(function()
		{
			test.failed('TCPsocket read/write test timed out after 2 seconds');
		},2000);
	}
});
