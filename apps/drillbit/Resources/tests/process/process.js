describe("process tests",
{
	test_process_object: function()
	{
		value_of(Titanium.Process).should_not_be_null();
		value_of(Titanium.Process.createProcess).should_be_function();
		value_of(Titanium.Process.createInputPipe).should_be_function();
		value_of(Titanium.Process.createOutputPipe).should_be_function();
		value_of(Titanium.Process.getCurrentProcess).should_be_function();
	},
	
	test_input_pipe: function()
	{
		var i = Titanium.Process.createInputPipe();
		value_of(i).should_be_object();
		value_of(i.read).should_be_function();
		value_of(i.split).should_be_function();
		value_of(i.unsplit).should_be_function();
		value_of(i.attach).should_be_function();
		value_of(i.detach).should_be_function();
		value_of(i.join).should_be_function();
		value_of(i.unjoin).should_be_function();
		value_of(i.isClosed).should_be_function();
		value_of(i.close).should_be_function();
		
		value_of(i.isClosed()).should_be_false();
		i.close();
		value_of(i.isClosed()).should_be_true();
	},
	
	test_input_pipe_empty_read: function() {
		var i = Titanium.Process.createInputPipe();
		var exception = false;
		var data = null;
		try { data = i.read(); } catch(e) { exception = true; }
		
		value_of(exception).should_be_false();
		value_of(data).should_be_object();
		value_of(data.length).should_be(0);
		
	},
	
	test_output_pipe: function()
	{
		var o = Titanium.Process.createOutputPipe();
		value_of(o).should_be_object();
		value_of(o.write).should_be_function();
		value_of(o.isClosed).should_be_function();
		value_of(o.close).should_be_function();
		
		value_of(o.isClosed()).should_be_false();
		o.close();
		value_of(o.isClosed()).should_be_true();
	},
	
	test_output_pipe_write: function()
	{
		var o = Titanium.Process.createOutputPipe();
		var blob = Titanium.CoreTypes.createBlob("some data");
		var written = o.write(blob);
		value_of(written).should_be(blob.length);
		o.close();
	},
	
	test_current_process: function()
	{
		var p = Titanium.Process.getCurrentProcess();
		value_of(p).should_not_be_null();
		value_of(p.getEnvironment).should_be_function();
		value_of(p.setEnvironment).should_be_function();
		value_of(p.cloneEnvironment).should_be_function();
		value_of(p.getArguments).should_be_function();
		value_of(p.stdin).should_be_null();
		value_of(p.stdout).should_be_null();
		value_of(p.stderr).should_be_null();
	},
	
	test_current_process_env: function()
	{
		var p = Titanium.Process.getCurrentProcess();
		value_of(p.getEnvironment("foobar")).should_be_undefined();
		p.setEnvironment("foobar", "1");
		value_of(p.getEnvironment("foobar")).should_be("1");
		var e = p.cloneEnvironment();
		value_of(e["foobar"]).should_be("1");
	},
	
	test_clone_environment: function()
	{
		var ce = Titanium.Process.getCurrentProcess().cloneEnvironment();
		var e = Titanium.Process.getCurrentProcess().getEnvironment();
		
		for (name in e) {
			value_of(name in ce).should_be_true();
			value_of(ce[name]).should_be(e[name]);
		}
	},
	
	test_current_process_arguments: function()
	{
		var p = Titanium.Process.getCurrentProcess();
		
		value_of(p.getArguments().length).should_be(Titanium.App.arguments.length+1);
		for (var i = 1; i < p.getArguments().length; i++)
		{
			value_of(Titanium.App.arguments).should_contain(p.getArguments()[i]);
		}
	},
	
	test_process_as_async: function(test)
	{
		var p = null;
		
		if (Titanium.platform == 'win32')
		{
			p = Titanium.Process.createProcess(['C:\\Windows\\system32\\cmd.exe', '/C', 'dir']);
		}
		else
		{
			p = Titanium.Process.createProcess(['/bin/ls', '-la']);
		}
		
		var timer = null;
		var shortTimer = null;
		value_of(p).should_not_be_null();
		var output = '';
		
		p.setOnRead(function(event)
		{
			var buf = event.pipe.read();
			output += buf;
		});
		
		p.setOnExit(function()
		{
			clearTimeout(timer);
			if (output.length > 0)
			{
				test.passed();
			}
			else
			{
				test.failed('no output received');
			}
		});
		
		p.launch();
		
		// if we hit this timeout, then we fail.		
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	},
	
	test_process_exception_as_async: function(test)
	{
		var p = null;
		if (Titanium.platform == 'win32')
		{
			p = Titanium.Process.createProcess(['C:\\Windows\\system32\\cmd.exe', '/c', 'dir']);
		}
		else
		{
			p = Titanium.Process.createProcess(['/bin/ls', '-la']);
		}

		var timer = null;
		value_of(p).should_not_be_null();
		var output = '';
		var throwException = true;
		
		p.setOnRead(function(event)
		{
			// test throwing exception from onread
			if (throwException) {
				throw "this is an exception";
				throwException = false;
			}
		});
		p.setOnExit(function()
		{
			clearTimeout(timer);
			test.passed();
		});
		
		p.launch();
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	}
});
