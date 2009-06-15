describe("file-based ajax tests",
{
	local_file_using_app_as_async:function(callback)
	{
		var timer = 0;
		
		$.getJSON('app://test.js',function(data)
		{
			clearTimeout(timer);
			try
			{
				value_of(data).should_be_object();
				value_of(data.success).should_be_true();
				value_of(data.abc).should_be(123);
				callback.passed();
			}
			catch(e)
			{
				callback.failed(e);
			}
		});

		// fail test after 2s
		timer = setTimeout(function()
		{
			callback.failed('ajax request timed out after 30s');
		},2000)		
	},
	local_file_using_file_as_async:function(callback)
	{
		var timer = 0;
		var js = Titanium.Filesystem.getFile(Titanium.App.appURLToPath('app://test.js'));
		
		$.getJSON('file://'+js.nativePath(),function(data)
		{
			clearTimeout(timer);
			try
			{
				value_of(data).should_be_object();
				value_of(data.success).should_be_true();
				value_of(data.abc).should_be(123);
				callback.passed();
			}
			catch(e)
			{
				callback.failed(e);
			}
		});

		// fail test after 2s
		timer = setTimeout(function()
		{
			callback.failed('ajax request timed out after 30s');
		},2000)		
	},
	test_query_string_as_async: function(callback)
	{
		var timer = 0;
		$.getJSON('app://url.js?q=1', function(data)
		{
			clearTimeout(timer);
			try
			{
				value_of(data).should_be_object();
				value_of(data.success).should_be_true();
				value_of(data.abc).should_be(123);
				callback.passed();
			}
			catch(e)
			{
				callback.failed(e);
			}
		});
		
		timer = setTimeout(function()
		{
			callback.failed('ajax request timed out after 2s');
		},2000);
	}
});