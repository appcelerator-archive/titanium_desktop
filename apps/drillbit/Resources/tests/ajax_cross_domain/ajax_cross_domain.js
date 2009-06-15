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
				try
				{
					value_of(data).should_be_object();
					callback.passed();
				}
				catch(e)
				{
					callback.failed(e);
				}
			},
			error: function(xhr,status,error)
			{
				callback.failed(error);
			}
		});

		// fail test after 30s
		timer = setTimeout(function()
		{
			callback.failed('ajax request timed out after 30s');
		},30000)		
	}
});