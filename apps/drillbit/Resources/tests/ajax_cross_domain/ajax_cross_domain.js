describe("ajax cross domain test",
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
		},30000)		
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
	}
});
