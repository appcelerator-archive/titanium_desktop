describe("UI Module Tests",{
	test_user_window_title: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('http://www.google.com');

		var title = "blahahahahahahahahahahahahaha";
		w.setTitle(title);
		value_of(w.getTitle()).should_be(title);
		title = "    ___       ";
		w.setTitle(title);
		value_of(w.getTitle()).should_be(title);
		title = "    _¬˚∆¬˚¬∂ƒ¬∆ø´ˆ∆¬ßƒ∂ˆ__       ";
		w.setTitle(title);
		value_of(w.getTitle()).should_be(title);
		title = "";
		w.setTitle(title);
		value_of(w.getTitle()).should_be(title);

		w.open();

		title = "blahahahahahahahahahahahahaha";
		w.setTitle(title);
		value_of(w.getTitle()).should_be(title);
		title = "    ___       ";
		w.setTitle(title);
		value_of(w.getTitle()).should_be(title);
		title = "    _¬˚∆¬˚¬∂ƒ¬∆ø´ˆ∆¬ßƒ∂ˆ__       ";
		w.setTitle(title);
		value_of(w.getTitle()).should_be(title);
		title = "";
		w.setTitle(title);
		value_of(w.getTitle()).should_be(title);

		w.close();
	},

	test_user_window_url: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('http://www.google.com');

		var url = "http://www.google.com";
		w.setURL(url);
		value_of(w.getURL()).should_be(url);
		url = "http://gmail.com";
		w.setURL(url);
		value_of(w.getURL()).should_be(url);
		url = "http://www.bbc.co.uk";
		w.setURL(url);
		value_of(w.getURL()).should_be(url);
		url = "http://mail.google.com";
		w.setURL(url);
		value_of(w.getURL()).should_be(url);

		w.open();

		url = "http://www.google.com";
		w.setURL(url);
		value_of(w.getURL()).should_be(url);
		url = "http://gmail.com";
		w.setURL(url);
		value_of(w.getURL()).should_be(url);
		url = "http://www.bbc.co.uk";
		w.setURL(url);
		value_of(w.getURL()).should_be(url);
		url = "http://mail.google.com";
		w.setURL(url);
		value_of(w.getURL()).should_be(url);
		w.close();
	},
	test_event_name_constants: function()
	{
		// If these tests fail, perhaps the constant has changed. If that's
		// the case, just update the tests below and be sure that nothing
		// else in the system breaks! Hopefully apps do not rely on magic strings.
		value_of(Titanium.UI.FOCUSED).should_be("focused");
		value_of(Titanium.UI.UNFOCUSED).should_be("unfocused");
		value_of(Titanium.UI.OPEN).should_be("open");
		value_of(Titanium.UI.OPENED).should_be("opened");
		value_of(Titanium.UI.CLOSE).should_be("close");
		value_of(Titanium.UI.CLOSED).should_be("closed");
		value_of(Titanium.UI.HIDDEN).should_be("hidden");
		value_of(Titanium.UI.SHOWN).should_be("shown");
		value_of(Titanium.UI.FULLSCREENED).should_be("fullscreened");
		value_of(Titanium.UI.UNFULLSCREENED).should_be("unfullscreened");
		value_of(Titanium.UI.MAXIMIZED).should_be("maximized");
		value_of(Titanium.UI.MINIMIZED).should_be("minimized");
		value_of(Titanium.UI.RESIZED).should_be("resized");
		value_of(Titanium.UI.MOVED).should_be("moved");
		value_of(Titanium.UI.PAGE_INITIALIZED).should_be("page.init");
		value_of(Titanium.UI.PAGE_LOADED).should_be("page.load");
	},
	test_window_listener_as_async: function(callback)
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');

		var messageLog = { };
		messageLog[Titanium.UI.CLOSE]  = false;
		messageLog[Titanium.UI.CLOSED]  = false
		messageLog[Titanium.UI.OPEN]  = false;
		messageLog[Titanium.UI.OPENED]  = false;
		messageLog[Titanium.UI.HIDDEN]  = false;

		w.addEventListener(function(eventName, eventData)
		{
			messageLog[eventName] = true;
		});

		w.open();
		setTimeout(function()
		{
			w.close();
			if (messageLog[Titanium.UI.OPEN] !== true)
			{
				callback.failed("Did not detect open message");
			}
			else if (messageLog[Titanium.UI.OPENED] !== true)
			{
				callback.failed("Did not detect opened message");
			}
			else
			{
				callback.passed();
			}
		}, 1000);
	},
	test_window_maximize_fires_resize_as_async: function(callback)
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		w.open();

		var resizedFired = false;
		var maximizedFired = false;

		w.addEventListener(function(eventName, eventData)
		{
			if (eventName == "resized")
			{
				resizedFired = true;
			}
			else if (eventName == "maximized")
			{
				maximizedFired = true;
			}
		});

		var stageTwo = function()
		{
			w.close();
			if (!maximizedFired)
			{
				callback.failed("Did not detect maximized message");
			}
			if (!resizedFired)
			{
				callback.failed("Did not detect resized message");
			}
			callback.passed();
		}

		setTimeout(function()
		{
			w.maximize();
			setTimeout(function()
			{
				stageTwo();
			}, 300);
		}, 300);


	}
});
