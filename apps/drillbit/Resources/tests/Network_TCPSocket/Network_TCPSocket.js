describe("Network.TCPSocket",{
	// ti.network.HTTP objects are covered in the http_server unit test.

	before_all: function()
	{
		// Launch testing server on port 8080
		/*this.testServer = Titanium.Process.createProcess(
		{
			args: [
				'python', Titanium.API.application.resourcesPath + "/testserver.py"
			],
		});
		this.testServer.launch();*/
	},
	
	after_all: function()
	{
		//this.testServer.kill();
	},

	before: function()
	{
		// Create a test socket client
		this.socket = Titanium.Network.createTCPSocket("127.0.0.1", 8080);
	},

	// test the network object and properties.
	test_TCPSocket_object: function()
	{
		value_of(this.socket).should_be_object();	
		value_of(this.socket.close).should_be_function();
		value_of(this.socket.connect).should_be_function();
		value_of(this.socket.isClosed).should_be_function();
		value_of(this.socket.write).should_be_function();
	},
	
	test_connect_as_async: function(test)
	{
		var timer;
		var socket = this.socket;

		socket.on("connect", function()
		{
			clearTimeout(timer);
			socket.close();
			test.passed();
		});

		socket.on("error", function(err)
		{
			clearTimeout(timer);
			test.failed(err);
		});

		socket.connect();
		timer = setTimeout(function()
		{
			test.failed("Test timed out");
		}, 2000);
	},

	test_timeout_as_async: function(test)
	{
		var timer;
		var socket = this.socket;

		socket.setTimeout(1000);
		socket.on("timeout", function()
		{
			clearTimeout(timer);
			socket.close();
			test.passed();
		});

		socket.connect();
		timer = setTimeout(function()
		{
			test.failed("Test timed out");
		}, 2000);
	},

	test_read_write_as_async: function(test)
	{
		var timer;
		var message = "hello, can anyone hear me?";
		var socket = this.socket;

		socket.on("connect", function()
		{
			// Send test server a message.
			socket.write(message);
		});

		socket.on("data", function(data)
		{
			clearTimeout(timer);

			try
			{
				// Test server should echo the message back.
				value_of(data).should_be(message);
				socket.close();
				test.passed();
			}
			catch (e)
			{
				test.failed(e);
			}
		});

		socket.on("error", function(err)
		{
			clearTimeout(timer);
			test.failed(err);
		});

		socket.connect();
		timer = setTimeout(function()
		{
			test.failed("Test timed out");
		}, 2000);
	}
});
