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


        value_of(p.command).should_not_be_null();
        value_of(p.err).should_not_be_null();
        value_of(p['in']).should_not_be_null();
        value_of(p['out']).should_not_be_null();
        value_of(p.terminate).should_be_function();
        value_of(p.pid).should_be_object();
        
		p.onread = function(buf)
		{
		    // reality check... are we running?
		    // this method is called when the process is executing
            value_of(this.running).should_be_true();
            
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

        value_of(p.command).should_not_be_null();
        value_of(p.err).should_not_be_null();
        value_of(p['in']).should_not_be_null();
        value_of(p['out']).should_not_be_null();
        value_of(p.terminate).should_be_function();
        value_of(p.pid).should_be_object();
        
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
