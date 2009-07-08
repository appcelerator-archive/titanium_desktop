describe("process exitCode tests",
{
	before_all: function()
	{
		var exitCodeSuccess = -1;
		var exitCodeFail = -1;
	},
	
	after_all: function()
	{
		// test for the difference in exitCode functions to complete
		// the test of the exitcode.  exitCodeSuccess should be 0
		// exitCodeFail should be non-zero.
		value_of(exitCodeSuccess != exitCodeFail).should_be_true();
		
	},
	
	test_process_exitCode_success_as_async: function(test)
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
		value_of(p).should_not_be_null();
		
		p.onread = function(buf)
		{
			value_of(this.exitCode).should_be_null();
		};

		p.onexit = function()
		{
			try 
			{
				exitCodeSuccess = this.exitCode;
				Titanium.API.debug("exitCode = "+this.exitCode);
				test.passed();
			}
			catch(e)
			{
				test.failed('exitCode not defined in onexit().');
			}
   			clearTimeout(timer);
		};

		// if we hit this timeout, then we fail.		
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	},
		
	test_process_exitCode_fail_as_async: function(test)
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
		value_of(p).should_not_be_null();
		
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
			try 
			{
				exitCodeFail = this.exitCode;
				Titanium.API.debug("exitCode = "+this.exitCode);
				test.passed();
			}
			catch(e)
			{
				test.failed('exitCode not defined in onexit().');
			}
   			clearTimeout(timer);
		};

		// if we hit this timeout, then we fail.		
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	}	
});
