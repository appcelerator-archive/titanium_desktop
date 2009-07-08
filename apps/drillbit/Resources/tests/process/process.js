describe("process tests",
{
	test_process_object: function()
	{
		value_of(Titanium.Process).should_not_be_null();
		value_of(Titanium.Process.getEnv).should_be_function();
		value_of(Titanium.Process.hasEnv).should_be_function();
		value_of(Titanium.Process.launch).should_be_function();
		value_of(Titanium.Process.pid).should_be_number();
		value_of(Titanium.Process.restart).should_be_function();
		value_of(Titanium.Process.setEnv).should_be_function();

		// are these tests even valid?
		value_of(Titanium.Process.Pipe).should_not_be_null();
		value_of(Titanium.Process.Process).should_not_be_null();


	},
	
	test_process_env: function()
	{
		// setup an environment variable
		Titanium.Process.setEnv("Foo", "bar");
		value_of(Titanium.Process.hasEnv("Foo")).should_be_true();
		value_of(Titanium.Process.getEnv("Foo")).should_be("bar");
		
	},

	test_process_as_async: function(test)
	{
		value_of(Titanium.Process).should_not_be_null();
		var p = null;
		
		if (Titanium.platform == 'win32')
		{
			p = Titanium.Process.launch('C:\\Windows\\system32\\cmd.exe',['/C', 'dir']);
		}
		else
		{
			p = Titanium.Process.launch('/bin/ls',['-la']);
		}

		var timer = null;
		var shortTimer = null;
		value_of(p).should_not_be_null();
		var output = '';
		
		p.onread = function(buf)
		{
			output += buf;
		};

		p.onexit = function()
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
		};
		
		// if we hit this timeout, then we fail.		
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	},
	
	test_process_exception_as_async: function(test)
	{
		value_of(Titanium.Process).should_not_be_null();
		var p = null;
		
		if (Titanium.platform == 'win32')
		{
			p = Titanium.Process.launch('C:\\Windows\\system32\\cmd.exe',['/c', 'dir']);
		}
		else
		{
			p = Titanium.Process.launch('/bin/ls',['-la']);
		}

		var timer = null;
		value_of(p).should_not_be_null();
		var output = '';
		var throwException = true;
		
		p.onread = function(buf)
		{
			// test throwing exception from onread
			if (throwException) {
				throw "this is an exception";
				throwException = false;
			}
		};
		p.onexit = function()
		{
			clearTimeout(timer);
			test.passed();
		};
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	}
});
