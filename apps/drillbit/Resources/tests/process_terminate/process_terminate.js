describe("process terminate tests",
{
	test_terminate_process_as_async: function(test)
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
		shortTimer = setTimeout(function()
		{
			// is the process running?
			if ( p.running )
			{
				// kill the process if it's running
				p.terminate();
				
				value_of(p.exitCode).should_not_be_null();
			}
			else 
			{
				// this should never happen.
				value_of(p.running).should_be_true();
			}
		}, 1000);
		
		// if we hit this timeout, then we fail.		
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	}
});
