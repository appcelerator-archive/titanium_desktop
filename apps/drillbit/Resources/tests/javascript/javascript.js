describe("javascript",
{
	test_javascript_functions_no_window: function()
	{
		value_of(Titanium.setTimeout).should_be_function();
		value_of(Titanium.setInterval).should_be_function();
		value_of(Titanium.clearTimeout).should_be_function();
		value_of(Titanium.clearInterval).should_be_function();
		
		value_of(Titanium.setTimeout).should_not_be(setTimeout);
		value_of(Titanium.setInterval).should_not_be(setInterval);
		value_of(Titanium.clearTimeout).should_not_be(clearTimeout);
		value_of(Titanium.clearInterval).should_not_be(clearInterval);
	},
	
	test_setTimeout_as_async: function(callback)
	{
		var timer = 0;
		Titanium.setTimeout(function() {
			try
			{
				value_of(clearTimeout).should_not_be(Titanium.clearTimeout);
			}
			catch (e)
			{
				callback.failed(e);
			}
			
			clearTimeout(timer);
			callback.passed();
		}, 500);
		
		timer = setTimeout(function() {
			callback.failed("timed out waiting for native setTimeout");
		}, 2000);
	},
	
	test_setInterval_as_async: function(callback)
	{
		var times = 0;
		var timer = 0;
		Titanium.setInterval(function() {
			times++;
			if (times == 5)
			{
				clearTimeout(timer);
				callback.passed();
			}
		}, 500);
		
		timer = setTimeout(function() {
			callback.failed("timed out waiting for 5 calls from native setInterval");
		}, 4000);
	},
	
	test_clearTimeout_as_async: function(callback)
	{
		var timer = Titanium.setTimeout(function() {
			callback.failed("timer wasn't cleared correctly");
		}, 500);
		
		Titanium.clearTimeout(timer);
		var timer2 = Titanium.setTimeout(function() {
			callback.passed();
		}, 1500);
	},
	
	test_clearInterval_as_async: function(callback)
	{
		var times = 0;
		var timer = Titanium.setInterval(function() {
			times++;
			if (times == 5)
			{
				Titanium.clearInterval(timer);
			}
			else if (times > 5)
			{
				callback.failed("interval wasn't cleared correctly");
			}
		}, 500);
		
		var timer2 = Titanium.setTimeout(function() {
			callback.passed();
		}, 4000);
	}
});