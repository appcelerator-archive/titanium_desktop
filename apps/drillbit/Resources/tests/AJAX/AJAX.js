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
	local_file_fail_using_app_as_async:function(callback)
	{
		// fail test after 2s
		var timer = setTimeout(
			function() { callback.failed('ajax request timed out after 30s'); },2000);

		$.ajax({
			url: 'app://testasdfasdfasdf.js',
			success: function(data)
			{
				clearTimeout(timer);
				callback.failed("Request succeeded, but should have failed");
			},
			error: function()
			{
				clearTimeout(timer);
				callback.passed();
			}
		});
	},
	local_file_using_app_as_async:function(callback)
	{
		// fail test after 2s
		var timer = setTimeout(function() { callback.failed('ajax request timed out after 30s'); },2000);

		$.ajax({
			url: 'app://test.js',
			success: function(data)
			{
				clearTimeout(timer);
				callback.passed();
			},
			error: function()
			{
				clearTimeout(timer);
				callback.failed("Request failed");
			}
		});
	},
	test_query_string_as_async: function(callback)
	{
		var timer = setTimeout(function() { callback.failed('ajax request timed out after 30s'); },2000);
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
	}
});
