describe("UI Module Tests",{
	test_user_window_title: function()
	{
	    // get the current UserWindow object
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
	    // get the current UserWindow object
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
	    // get the current UserWindow object
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');

		var messageLog = { };
		messageLog[Titanium.UI.CLOSE]  = false;
		messageLog[Titanium.UI.CLOSED]  = false;
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
	    // get the current UserWindow object
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
		};

		setTimeout(function()
		{
			w.maximize();
			setTimeout(function()
			{
				stageTwo();
			}, 300);
		}, 300);
	},
	
	test_constants: function()
	{
		value_of(Titanium.UI.CENTERED).should_be_number();
		value_of(Titanium.UI.FOCUSED).should_be_string();
		value_of(Titanium.UI.UNFOCUSED).should_be_string();
		value_of(Titanium.UI.OPEN).should_be_string();
		value_of(Titanium.UI.OPENED).should_be_string();
		value_of(Titanium.UI.CLOSE).should_be_string();
		value_of(Titanium.UI.CLOSED).should_be_string();
		value_of(Titanium.UI.HIDDEN).should_be_string();
		value_of(Titanium.UI.SHOWN).should_be_string();
		value_of(Titanium.UI.FULLSCREENED).should_be_string();
		value_of(Titanium.UI.UNFULLSCREENED).should_be_string();
		value_of(Titanium.UI.MAXIMIZED).should_be_string();
		value_of(Titanium.UI.MINIMIZED).should_be_string();
		value_of(Titanium.UI.RESIZED).should_be_string();
		value_of(Titanium.UI.MOVED).should_be_string();
		value_of(Titanium.UI.PAGE_INITIALIZED).should_be_string();
		value_of(Titanium.UI.PAGE_LOADED).should_be_string();
		value_of(Titanium.UI.CREATE).should_be_string();
	},
	
	test_window_events: function()
	{
		value_of(true).should_be_true();
		
		var events = [];
		var eventObject = 0;
		var windowEventListener = function(n,e)
		{
			events.unshift(n);
			eventObject = e;
		};
		
		var w = Titanium.UI.getCurrentWindow().createWindow('http://www.google.com');
		var listenerID = w.addEventListener(windowEventListener);
		value_of(listenerID).should_be_number();
		
		var test_event = function(e,idx)
		{
			if (!idx) idx = 0;
			
			value_of(events[0]).should_be(e);
			value_of(eventObject.window.equals(w)).should_be_true();
		};
		
		w.open();
		test_event(Titanium.UI.OPENED);
		
		w.setVisible(false);
		test_event(Titanium.UI.HIDDEN);
		
		w.setVisible(true);
		test_event(Titanium.UI.SHOWN);
		
		w.setFullScreen(true);
		test_event(Titanium.UI.FULLSCREENED);
		
		w.setFullScreen(false);
		test_event(Titanium.UI.UNFULLSCREENED);
		
		w.maximize();
		test_event(Titanium.UI.MAXIMIZED);
		
		w.minimize();
		test_event(Titanium.UI.MINIMIZED);
		
		var b = w.getBounds();
		w.setX(b.x+1);
		test_event(Titanium.UI.MOVED);
		
		w.setY(b.y+1);
		test_event(Titanium.UI.MOVED);
		
		w.setWidth(b.width+1);
		test_event(Titanium.UI.RESIZED);
		
		w.setHeight(b.height+1);
		test_event(Titanium.UI.RESIZED);
		
		b.height+=2;
		b.x+=5;
		w.setBounds(b);
		test_event(Titanium.UI.RESIZED);
		
		w.close();
		test_event(Titanium.UI.CLOSE);
	}
});
