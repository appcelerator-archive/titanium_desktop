describe("Network.TCPSocket",{
	// ti.network.HTTP objects are covered in the http_server unit test.

	before_all: function()
	{
		value_of(Titanium.Network).should_not_be_null();
		value_of(Titanium.Network.createTCPSocket).should_be_function();
		value_of(Titanium.Network.TCPSocket).should_not_be_null();

		// Launch testing server on port 8080
		// Launch test http server
		this.testServer = Titanium.Process.createProcess(
		{
			args: [
				'python', Titanium.API.application.resourcesPath + "/testserver.py"
			],
		});
		this.testServer.launch();
		
		// create a socket to the localhost web server.
		this.socket = Titanium.Network.createTCPSocket("127.0.0.1", 8080);
	},
	
	after_all: function()
	{
		this.socket = null;
		this.testServer.kill();
	},

	// test the network object and properties.
	test_TCPSocket_object:function()
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
	
	test_isClosed: function()
	{
		value_of(this.socket.isClosed).should_be_function();
		value_of(this.socket.isClosed()).should_be_true();
	},
	
	test_connect_as_async: function(test)
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

	test_read_write_as_async: function(test)
	{
		var socket = this.socket;
		var testText = "Can anyone hear me???";
		var buffer = "";
		var timer;

		socket.onRead( function(data)
		{
			buffer += data;
		});

		socket.onReadComplete( function()
		{
			clearTimeout(timer);

			if (buffer != testText)
			{
				test.failed("Data received back does not match what was sent: " + buffer);
			}
			else
			{
				test.failed("data: " + buffer);
			}

			socket.close();
		});
		
		socket.onTimeout( function(buf)
		{
			clearTimeout(timer);
			test.failed("Socket timed out");
		});
		
		try 
		{
			value_of(socket.connect()).should_be_true();		
		}
		catch(e)
		{
			// during testing I found that this exception 
			// will never get hit because we don't intercept the 
			// exception here, only drillbit seems capable enough
			// to handle this.  kind of sucks since I could use this as a test.
			test.failed("failed with exception on connect"+e);
		}

		try
		{
			value_of(socket.write(testText)).should_be_true();
		}
		catch(e)
		{
			test.failed("Exception thrown while writing: " + e);			
		}

		timer = setTimeout(function()
		{
			test.failed('Test timed out');
		}, 2000);
	}
});
