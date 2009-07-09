describe("process running tests",
{
	test_process_object_as_async: function(test)
	{
		value_of(Titanium.Process).should_not_be_null();
		var p = null;
		
		if (Titanium.platform == 'win32')
		{
			// open a shell and leave it open
			p = Titanium.Process.launch('C:\\Windows\\system32\\cmd.exe',['/K', 'dir']);
		}
		else
		{
			// on non-windows platforms, we launch using CAT to just let it sit and wait for input.
			p = Titanium.Process.launch('/bin/cat',['-v']);
		}

		var timer = null;
		var shortTimer = null;
		value_of(p).should_not_be_null();

		value_of(p.command).should_not_be_null();
		value_of(p.err).should_not_be_null();
		value_of(p['in']).should_not_be_null();
		value_of(p['out']).should_not_be_null();
		value_of(p.terminate).should_be_function();
		
		// in the process object we init the pid to NULL, then assign an int later.
		value_of(p.pid).should_not_be_null();
		value_of(p.pid).should_be_number();
		
		p.onread = function(buf)
		{
			// reality check... are we running?
			value_of(this.running).should_be_true();
		};

		p.onexit = function()
		{
			clearTimeout(timer);

			// we should not be running on exit.  
			if (this.running)
			{
				test.failed('test is running onexit, should be running');
			}
			else
			{
				test.passed();
			}
		};
		shortTimer = setTimeout(function()
		{
			// reality check... are we running?
			value_of(p.running).should_be_true();
		
			// kill the process if it's running
			p.terminate();
		}, 1000);

		// if we hit this timeout, then we fail.		
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	}
});
