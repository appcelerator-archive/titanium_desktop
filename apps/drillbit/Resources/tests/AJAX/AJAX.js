describe("WebKit AJAX",
{
	twitter_as_async:function(callback)
	{
		var timer = 0;
		
		$.ajax({
			type:'GET',
			dataType:'json',
			url:'http://twitter.com/statuses/public_timeline.json',

			success: function(data)
			{
				clearTimeout(timer);
				if (typeof(data) !== 'object')
				{
					callback.failed("HTTP Twitter request did not return object");
				}
				else
				{
					callback.passed();
				}
			},
			error: function(xhr,status,error)
			{
				var err = "HTTP Twitter request failed:";
				err += (status | error | "Unknown");
				callback.failed(err);
			}
		});

		// fail test after 30s
		timer = setTimeout(function()
		{
			callback.failed('ajax request timed out after 30s');
		},30000);
	},
	twitter_https_as_async:function(callback)
	{
		var timer = 0;
		$.ajax({
			type:'GET',
			dataType:'json',
			url:'https://twitter.com/statuses/public_timeline.json',

			success: function(data)
			{
				clearTimeout(timer);
				if (typeof(data) !== 'object')
				{
					callback.failed("HTTPS Twitter request did not return object");
				}
				else
				{
					callback.passed();
				}
			},
			error: function(xhr,status,error)
			{
				var err = "HTTPS Twitter request failed:";
				err += (status | error | "Unknown");
				callback.failed(err);
			}
		});

		// fail test after 30s
		timer = setTimeout(function()
		{
			callback.failed('ajax request timed out after 30s');
		},30000)		
	},
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
		$.getJSON('app://test.js?q=1', function(data)
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
