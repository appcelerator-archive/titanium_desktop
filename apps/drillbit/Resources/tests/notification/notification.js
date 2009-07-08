describe("Notification tests",
{
	test_notification_object: function()
	{
		var wnd = Titanium.Notification.createNotification(parent.window);
		
		value_of(wnd.hide).should_be_function();
		value_of(wnd.setCallback).should_be_function();
		value_of(wnd.setDelay).should_be_function();
		value_of(wnd.setIcon).should_be_function();
		value_of(wnd.setMessage).should_be_function();
		value_of(wnd.setTitle).should_be_function();
		value_of(wnd.show).should_be_function();	   
		value_of(wnd).should_be_object();	
	},
	
	test_it_with_large_icon_as_async: function(callback)
	{
		value_of(Titanium.Notification.createNotification).should_be_function();
		
		// create a notification object
		var parent = Titanium.UI.getCurrentWindow();
		var wnd = Titanium.Notification.createNotification(parent.window);
		wnd.setTitle("title");
		wnd.setMessage("this is a message with a big icon");
		wnd.setIcon("app://logo_large.png");
		wnd.setDelay(5000);
		
		wnd.setCallback(function () {
			Titanium.API.debug("user click on notification window");
			callback.passed();
		});
		
		
		Titanium.API.debug("attempting to show the notification");
		wnd.show();

		timer = setTimeout(function()
		{
			try 
			{
				Titanium.API.debug("hiding the notification");
				wnd.hide();
				callback.passed();
			}
			catch(e)
			{
				Titanium.API.debug("hiding the notification failed with an exception");
				callback.failed();
			}
		},1000);
	},

	test_it_with_small_icon_as_async: function(callback)
	{
		value_of(Titanium.Notification.createNotification).should_be_function();
		
		// create a notification object
		var parent = Titanium.UI.getCurrentWindow();
		var wnd = Titanium.Notification.createNotification(parent.window);
		wnd.setTitle("title");
		wnd.setMessage("this is a message with a small icon");
		wnd.setIcon("app://logo_small.png");
		wnd.setDelay(5000);
		
		wnd.setCallback(function () {
			Titanium.API.debug("user click on notification window");
			callback.passed();
		});
		
		
		Titanium.API.debug("attempting to show the notification");
		wnd.show();

		timer = setTimeout(function()
		{
			try 
			{
				Titanium.API.debug("hiding the notification");
				wnd.hide();
				callback.passed();
			}
			catch(e)
			{
				Titanium.API.debug("hiding the notification failed with an exception");
				callback.failed();
			}
		},1000);
	}
});
