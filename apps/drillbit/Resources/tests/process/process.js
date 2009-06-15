describe("process tests",
{
	test_process_as_async: function(test)
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
