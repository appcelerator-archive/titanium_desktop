describe("UI Module Tests",{
	test_ui_module_methods: function()
	{
		value_of(Titanium.UI.addTray).should_be_function();
		value_of(Titanium.UI.clearTray).should_be_function();
		value_of(Titanium.UI.createMenu).should_be_function();
		value_of(Titanium.UI.getContextMenu).should_be_function();
		value_of(Titanium.UI.getIdleTime).should_be_function();
		value_of(Titanium.UI.getMenu).should_be_function();
		value_of(Titanium.UI.mainWindow).should_be_object();
		value_of(Titanium.UI.setContextMenu).should_be_function();
		value_of(Titanium.UI.setDockIcon).should_be_function();
		value_of(Titanium.UI.setDockMenu).should_be_function();
		value_of(Titanium.UI.setIcon).should_be_function();
		value_of(Titanium.UI.setMenu).should_be_function();
		value_of(Titanium.UI.setBadge).should_be_function();
		value_of(Titanium.UI.setBadgeImage).should_be_function();
	},
	test_windows_array: function()
	{
		value_of(Titanium.UI.getOpenWindows()).should_be_object();
		value_of(Titanium.UI.getOpenWindows().length).should_be(1);

		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		value_of(Titanium.UI.getOpenWindows().length).should_be(1);
		w.open();
		value_of(Titanium.UI.getOpenWindows().length).should_be(2);
		value_of(Titanium.UI.getCurrentWindow().equals(w.getParent())).should_be_true();

		var w2 = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		value_of(Titanium.UI.getOpenWindows().length).should_be(2);
		w2.open();
		value_of(Titanium.UI.getOpenWindows().length).should_be(3);
		value_of(Titanium.UI.getCurrentWindow().equals(w2.getParent())).should_be_true();
		value_of(w2.getParent().equals(w.getParent())).should_be_true();
		value_of(w2 != w).should_be_true();
	},
	test_window_max_size: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		w.setHeight(700);
		w.setWidth(700);

		value_of(w.getHeight()).should_be(700);
		value_of(w.getWidth()).should_be(700);

		w.setMaxHeight(500);
		value_of(w.getHeight()).should_be(500);

		w.setMaxWidth(400);
		value_of(w.getWidth()).should_be(400);

		w.setHeight(700);
		w.setWidth(700);
		value_of(w.getHeight()).should_be(500);
		value_of(w.getWidth()).should_be(400);

		w.setMaxWidth(-1);
		w.setMaxHeight(-1);
		w.setHeight(700);
		w.setWidth(700);

		w.open();

		value_of(w.getHeight()).should_be(700);
		value_of(w.getWidth()).should_be(700);

		w.setMaxHeight(500);
		value_of(w.getHeight()).should_be(500);

		w.setMaxWidth(400);
		value_of(w.getWidth()).should_be(400);

		w.setHeight(700);
		w.setWidth(700);
		value_of(w.getHeight()).should_be(500);
		value_of(w.getWidth()).should_be(400);

		value_of(w.getMaxHeight()).should_be(500);
		value_of(w.getMaxWidth()).should_be(400);
		
		w.close();
	},
	test_window_min_size: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		w.setHeight(100);
		w.setWidth(130);

		value_of(w.getHeight()).should_be(100);
		value_of(w.getWidth()).should_be(130);

		w.setMinHeight(500);
		value_of(w.getHeight()).should_be(500);

		w.setMinWidth(400);
		value_of(w.getWidth()).should_be(400);

		w.setHeight(100);
		w.setWidth(130);
		value_of(w.getHeight()).should_be(500);
		value_of(w.getWidth()).should_be(400);

		w.setMinWidth(-1);
		w.setMinHeight(-1);
		w.setHeight(100);
		w.setWidth(130);

		w.open();

		value_of(w.getHeight()).should_be(100);
		value_of(w.getWidth()).should_be(130);
		

		w.setMinHeight(500);
		value_of(w.getHeight()).should_be(500);

		w.setMinWidth(400);
		value_of(w.getWidth()).should_be(400);

		w.setHeight(100);
		w.setWidth(130);
		value_of(w.getHeight()).should_be(500);
		value_of(w.getWidth()).should_be(400);

		value_of(w.getMinHeight()).should_be(500);
		value_of(w.getMinWidth()).should_be(400);

		w.close();
	},
	test_window_set_height: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		w.setHeight(100);
		value_of(w.getHeight()).should_be(100);
		w.setHeight(200);
		value_of(w.getHeight()).should_be(200);
		w.setHeight(100);
		value_of(w.getHeight()).should_be(100);
		w.setHeight(10000);
		value_of(w.getHeight()).should_be(10000);
		w.setHeight(100);
		value_of(w.getHeight()).should_be(100);
		w.setHeight(-1);
		value_of(w.getHeight()).should_be(100);
		w.setHeight(0);
		value_of(w.getHeight()).should_be(100);
		w.open();
		w.setHeight(136);
		value_of(w.getHeight()).should_be(136);
		w.setHeight(200);
		value_of(w.getHeight()).should_be(200);
		w.setHeight(136);
		value_of(w.getHeight()).should_be(136);
		w.setHeight(10000);
		value_of(w.getHeight()).should_be(10000);
		w.setHeight(136);
		value_of(w.getHeight()).should_be(136);
		w.setHeight(-1);
		value_of(w.getHeight()).should_be(136);
		w.setHeight(-666);
		value_of(w.getHeight()).should_be(136);
		w.setHeight(0);
		value_of(w.getHeight()).should_be(136);
	},
	test_window_set_width: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		w.setWidth(100);
		value_of(w.getWidth()).should_be(100);
		w.setWidth(200);
		value_of(w.getWidth()).should_be(200);
		w.setWidth(100);
		value_of(w.getWidth()).should_be(100);
		w.setWidth(10000);
		value_of(w.getWidth()).should_be(10000);
		w.setWidth(100);
		value_of(w.getWidth()).should_be(100);
		w.setWidth(-1);
		value_of(w.getWidth()).should_be(100);
		w.setWidth(0);
		value_of(w.getWidth()).should_be(100);
		w.open()
		w.setWidth(136);
		value_of(w.getWidth()).should_be(136);
		w.setWidth(200);
		value_of(w.getWidth()).should_be(200);
		w.setWidth(136);
		value_of(w.getWidth()).should_be(136);
		w.setWidth(10000);
		value_of(w.getWidth()).should_be(10000);
		w.setWidth(136);
		value_of(w.getWidth()).should_be(136);
		w.setWidth(-1);
		value_of(w.getWidth()).should_be(136);
		w.setWidth(-666);
		value_of(w.getWidth()).should_be(136);
		w.setWidth(0);
		value_of(w.getWidth()).should_be(136);
	},
	test_window_set_closeable: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'http://blahblah.html', closeable: false});
		value_of(w.isCloseable()).should_be_false();
		w.setCloseable(true);
		value_of(w.isCloseable()).should_be_true();
		w.open();
		value_of(w.isCloseable()).should_be_true();
		w.setCloseable(false);
		value_of(w.isCloseable()).should_be_false();
		w.setCloseable(true);
		value_of(w.isCloseable()).should_be_true();
	},
	test_window_set_minimizable: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', minimizable: false});
		value_of(w.isMinimizable()).should_be_false();
		w.setMinimizable(true);
		value_of(w.isMinimizable()).should_be_true();
		w.open();
		value_of(w.isMinimizable()).should_be_true();
		w.setMinimizable(false);
		value_of(w.isMinimizable()).should_be_false();
		w.setMinimizable(true);
		value_of(w.isMinimizable()).should_be_true();
	},
	test_window_set_maximizable: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', maximizable: false});
		value_of(w.isMaximizable()).should_be_false();
		w.setMaximizable(true);
		value_of(w.isMaximizable()).should_be_true();
		w.open();
		value_of(w.isMaximizable()).should_be_true();
		w.setMaximizable(false);
		value_of(w.isMaximizable()).should_be_false();
		w.setMaximizable(true);
		value_of(w.isMaximizable()).should_be_true();
	},
	test_window_set_using_chrome: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', usingChrome: false});
		value_of(w.isUsingChrome()).should_be_false();
		w.setUsingChrome(true);
		value_of(w.isUsingChrome()).should_be_true();
		w.open();
		value_of(w.isUsingChrome()).should_be_true();
		w.setUsingChrome(false);
		value_of(w.isUsingChrome()).should_be_false();
		w.setUsingChrome(true);
		value_of(w.isUsingChrome()).should_be_true();
	},
	test_window_visibility_as_async: function(test)
	{
		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', visible: false});
		value_of(w.isVisible()).should_be_false();
		w.open();
		value_of(w.isVisible()).should_be_false();
		w.close();

		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', visible: true});
		value_of(w.isVisible()).should_be_false();
		w.setVisible(true);
		value_of(w.isVisible()).should_be_false();
		
		var timer = 0;
		// may not be visible right away, we need to add an event listener
		// for PAGE_LOADED
		w.addEventListener(Titanium.PAGE_LOADED, function(event)
		{
			clearTimeout(timer);
			try
			{
				value_of(w.isVisible()).should_be_true();
				w.setVisible(false);
				value_of(w.isVisible()).should_be_false();
		
				w.show();
				value_of(w.isVisible()).should_be_true();
				w.hide();
				value_of(w.isVisible()).should_be_false();
				test.passed();
			}
			catch (e)
			{
				test.failed(e);	
			}
		});
		
		timer = setTimeout(function() {
			test.failed("timed out waiting for page to initialize");
		}, 10000);
		w.open();
	},
	test_window_location: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', x: 100, y:200});
		value_of(w.getX()).should_be(100);
		value_of(w.getY()).should_be(200);

		w.setX(400);
		w.setY(300);
		value_of(w.getX()).should_be(400);
		value_of(w.getY()).should_be(300);

		w.open();
		value_of(w.getX()).should_be(400);
		value_of(w.getY()).should_be(300);

		w.setX(101);
		w.setY(153);
		value_of(w.getX()).should_be(101);
		value_of(w.getY()).should_be(153);

		w.setX(-1);
		w.setY(-2);
		value_of(w.getX()).should_be(-1);
		value_of(w.getY()).should_be(-2);

		w.setX(-666);
		w.setY(-333);
		value_of(w.getX()).should_be(-666);
		value_of(w.getY()).should_be(-333);
	},
	test_offscreen_window_locations: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', x: 100, y:200});
		value_of(w.getX()).should_be(100);
		value_of(w.getY()).should_be(200);

		w.setX(-1);
		w.setY(-2);
		value_of(w.getX()).should_be(-1);
		value_of(w.getY()).should_be(-2);

		w.setX(-666);
		w.setY(-333);
		value_of(w.getX()).should_be(-666);
		value_of(w.getY()).should_be(-333);

		// Take it to the max! Yeah!
		w.setX(-10000);
		w.setY(-10001);
		value_of(w.getX()).should_be(-10000);
		value_of(w.getY()).should_be(-10001);

		w.setX(100000);
		w.setY(200000);
		value_of(w.getX()).should_be(100000);
		value_of(w.getY()).should_be(200000);

		w.setX(-666);
		w.setY(-333);
		value_of(w.getX()).should_be(-666);
		value_of(w.getY()).should_be(-333);

		w.open();
		value_of(w.getX()).should_be(-666);
		value_of(w.getY()).should_be(-333);

		// Take it to the max! Yeah! Yeah!
		w.setX(-10000);
		w.setY(-10001);
		value_of(w.getX()).should_be(-10000);
		value_of(w.getY()).should_be(-10001);

		
		// max positive location in win32 (after being opened) is 32767x32767
		w.setX(100000);
		w.setY(200000);
		
		if (Titanium.platform != "win32") {
			value_of(w.getX()).should_be(100000);
			value_of(w.getY()).should_be(200000);
		}
		else {
			value_of(w.getX()).should_be(32767);
			value_of(w.getY()).should_be(32767);
		}
	},
	test_window_bounds: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow({
			url: 'app://blahblah.html',
			width: 444,
			height: 333,
			x: 100,
			y: 200});
		value_of(w.getX()).should_be(100);
		value_of(w.getY()).should_be(200);
		value_of(w.getWidth()).should_be(444);
		value_of(w.getHeight()).should_be(333);

		var b = w.getBounds();
		value_of(b.x).should_be(100);
		value_of(b.y).should_be(200);
		value_of(b.width).should_be(444);
		value_of(b.height).should_be(333);

		b.x = 555;
		b.y = 222;
		b.width = 500;
		b.height = 200;
		w.setBounds(b);

		var b = w.getBounds();
		value_of(b.x).should_be(555);
		value_of(b.y).should_be(222);
		value_of(b.width).should_be(500);
		value_of(b.height).should_be(200);
		value_of(w.getX()).should_be(555);
		value_of(w.getY()).should_be(222);
		value_of(w.getWidth()).should_be(500);
		value_of(w.getHeight()).should_be(200);

		w.setWidth(444);
		w.setHeight(333);
		w.setX(100);
		w.setY(200);
		w.open();

		value_of(w.getX()).should_be(100);
		value_of(w.getY()).should_be(200);
		value_of(w.getWidth()).should_be(444);
		value_of(w.getHeight()).should_be(333);

		var b = w.getBounds();
		value_of(b.x).should_be(100);
		value_of(b.y).should_be(200);
		value_of(b.width).should_be(444);
		value_of(b.height).should_be(333);

		b.x = 555;
		b.y = 222;
		b.width = 500;
		b.height = 200;
		w.setBounds(b);

		var b = w.getBounds();
		value_of(b.x).should_be(555);
		value_of(b.y).should_be(222);
		value_of(b.width).should_be(500);
		value_of(b.height).should_be(200);
		value_of(w.getX()).should_be(555);
		value_of(w.getY()).should_be(222);
		value_of(w.getWidth()).should_be(500);
		value_of(w.getHeight()).should_be(200);
	},
	test_window_maximize: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', maximized: true});
		value_of(w.isMaximized()).should_be(true);
		w.open();
		value_of(w.isMaximized()).should_be(true);
		w.close();

		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', maximized: false});
		value_of(w.isMaximized()).should_be(false);
		w.open();
		value_of(w.isMaximized()).should_be(false);
		w.close();

		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		value_of(w.isMaximized()).should_be(false);
		w.maximize();
		value_of(w.isMaximized()).should_be(true);
		w.open();
		value_of(w.isMaximized()).should_be(true);
		w.unmaximize();
		value_of(w.isMaximized()).should_be(false);
		w.maximize();
		value_of(w.isMaximized()).should_be(true);
		w.close();

		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		value_of(w.isMaximized()).should_be(false);
		w.maximize();
		value_of(w.isMaximized()).should_be(true);
		w.unmaximize();
		value_of(w.isMaximized()).should_be(false);
		w.open();
		value_of(w.isMaximized()).should_be(false);
		w.maximize();
		value_of(w.isMaximized()).should_be(true);
		w.unmaximize();
		value_of(w.isMaximized()).should_be(false);
		w.close();
	},
	test_window_minimize: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', minimized: true});
		value_of(w.isMinimized()).should_be(true);
		w.open();
		value_of(w.isMinimized()).should_be(true);
		w.close();

		var w = Titanium.UI.getCurrentWindow().createWindow({url: 'app://blahblah.html', minimized: false});
		value_of(w.isMinimized()).should_be(false);
		w.open();
		value_of(w.isMinimized()).should_be(false);
		w.close();

		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		value_of(w.isMinimized()).should_be(false);
		w.minimize();
		value_of(w.isMinimized()).should_be(true);
		w.open();
		value_of(w.isMinimized()).should_be(true);
		w.unminimize();
		value_of(w.isMinimized()).should_be(false);
		w.minimize();
		value_of(w.isMinimized()).should_be(true);
		w.close();

		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		value_of(w.isMinimized()).should_be(false);
		w.minimize();
		value_of(w.isMinimized()).should_be(true);
		w.unminimize();
		value_of(w.isMinimized()).should_be(false);
		w.open();
		value_of(w.isMinimized()).should_be(false);
		w.minimize();
		value_of(w.isMinimized()).should_be(true);
		w.unminimize();
		value_of(w.isMinimized()).should_be(false);
		w.close();
	},
	test_multi_open_as_async: function(callback)
	{
		var count = 5;
		var w = null;;
		function closeBlimpWindow()
		{
			w.close();
			setTimeout(function() { openBlimpWindow(); }, 200);
		}
		function openBlimpWindow()
		{
			if (count == 0)
			{
				callback.passed();
			}
			else
			{
				count = count - 1;
				w = Titanium.UI.getCurrentWindow().createWindow('app://multi_open.html');
				w.open();
				setTimeout(function() { closeBlimpWindow(); }, 200);
			}
		}
		openBlimpWindow();
	},
	test_title_override_as_async: function(callback)
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://multi_open.html');
		var w2 = Titanium.UI.getCurrentWindow().createWindow('app://multi_open.html');
		w.setTitle("Set1!");
		w.setTitle("Set2!");

		w.addEventListener(Titanium.PAGE_INITIALIZED, function(event) {
			w.setTitle("blah");
		});

		w.open();
		w2.open();
		setTimeout(function()
		{
			if (w.getTitle() != "blah") {
				w.close();
				callback.failed("Set title did not override header title");
			} else {
				w.close();
				callback.passed();
			}
			if (w2.getTitle() != "My fancy new title!") {
				w.close();
				callback.failed("Title tag did not override title");
			} else {
				w.close();
				callback.passed();
			}
		}, 1000);
	},
	test_close_message_on_originating_window_as_async: function(callback)
	{
		Titanium.saw_close = false;
		Titanium.saw_closed = false;

		var w = Titanium.UI.getCurrentWindow().createWindow('app://test_close_event_listener.html');
		w.open();

		setTimeout(function() {
			w.close();
			setTimeout(function() {
				if (Titanium.API.saw_close) {
					callback.passed();
				} else {
					callback.failed("Closing window did not receive CLOSE event");
				}
			}, 1000);
		}, 200);
	},
	test_window_top_most: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		
		w.open();
		value_of(w.isTopMost()).should_be(false);

		w.setTopMost(true);
		value_of(w.isTopMost()).should_be(true);

		w.setTopMost(false);
		value_of(w.isTopMost()).should_be(false);

		w.minimize();
		value_of(w.isMinimized()).should_be(true);
		value_of(w.isTopMost()).should_be(false);
		w.unminimize();

		w.setVisible(false);
		value_of(w.isVisible()).should_be_false();
		value_of(w.isTopMost()).should_be(false);

		w.show();
		value_of(w.isVisible()).should_be_true();
		value_of(w.isTopMost()).should_be(false);
		w.hide();
		value_of(w.isVisible()).should_be_false();
		value_of(w.isTopMost()).should_be(false);

		w.close();
	},
	
	test_window_resizable: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		value_of(w.isResizable()).should_be(true);
		w.open();

		value_of(w.isResizable()).should_be(true);

		w.setResizable(false);
		value_of(w.isResizable()).should_be(false);

		w.setResizable(true);
		value_of(w.isResizable()).should_be(true);

		w.close();
	},

	test_window_focus_as_async: function(callback)
	{
		var hasFocus = false;
		// get the current UserWindow object
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		var w2 = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		w.open();
		w2.open();

		w.addEventListener(function(event)
		{
			if (event.type == Titanium.FOCUSED)
			{
				hasFocus = true;
			}
		});

		w2.focus();
		setTimeout(function()
		{
			w.focus();
		}, 100);

		setTimeout(function()
		{
			w.close();
			w2.close();
			if (!hasFocus)
			{
				callback.failed("Did not detect focused message");
			}
			callback.passed();
		}, 500);
	},

	test_window_unfocus_as_async: function(callback)
	{
		var sawEvent = false;
		// get the current UserWindow object
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		var w2 = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		w.open();
		w2.open();

		// basically the same test as before, but we add the event listener to
		// the second window and wait for the unfocus event.
		w2.addEventListener(function(event)
		{
			if (event.type == Titanium.FOCUSED)
			{
				setTimeout(function() { w2.unfocus();}, 200);
			}
			if (event.type == Titanium.UNFOCUSED)
			{
				sawEvent = true;
			}
		});

		// just make sure we have the focus...
		w2.focus();
		setTimeout(function()
		{
			w.close();
			w2.close();
			if (!sawEvent)
			{
				callback.failed("Did not detect unfocused message");
			}
			callback.passed();
		}, 600);
	},
	test_window_mainWnd_by_ID: function()
	{
		var currentWnd = Titanium.UI.getCurrentWindow();
		var mainWnd = Titanium.UI.getMainWindow();
		value_of(mainWnd.getID() == currentWnd.getID()).should_be_true();

		var w = currentWnd.createWindow('app://blahblah.html');
		w.open();

		value_of(mainWnd == w).should_be_false();
		value_of(currentWnd.getID() == w.getID()).should_be_false();
		
		var whosUrDaddy = w.getParent();
		value_of(mainWnd.getID() == whosUrDaddy.getID()).should_be_true();
		
		w.close();
	},
	test_window_icon: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		w.open();

		// I expect that there is no icon defined for the test.
		if (w.getIcon() != null)
		{
			// if there is, then it should be returned as a string.
			value_of(w.getIcon()).should_be_string();
		}

		// setup a dummy icon
		w.setIcon("doesnotexist.png");
		value_of(w.getIcon()).should_be("doesnotexist.png");

		w.close();
	},
	test_window_getchildren: function()
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('app://blahblah.html');
		w.open();

		value_of(w.getChildren).should_be_function();
		var children = w.getChildren();
		value_of(children).should_be_object();
		value_of(children.length).should_be(0);

		var w2 = w.createWindow("app://blahblah.html");
		var w3 = w.createWindow("app://blahblah.html");
		w2.open();
		w3.open();

		var children = w.getChildren();
		value_of(children.length).should_be(2);
		value_of(children[0].equals(w2)).should_be_true();
		value_of(children[1].equals(w3)).should_be_true();

		w2.close();
		w3.close();
		w.close();
	},
	test_window_opener_not_present: function()
	{
		// window.opener should be null when a single window with no parent
		value_of(window.opener).should_be_null();
	},
	test_window_opener_as_async: function(callback)
	{
		Titanium.API.ui_test_opener_value = undefined;
		var w = Titanium.UI.getCurrentWindow().createWindow('app://test_window_opener.html');
		w.open();
		setTimeout(function()
		{
			var failed = true;
			try
			{
				value_of(Titanium.API.ui_test_opener_value).should_be_true();
			}
			catch(e)
			{
				callback.failed(e);
			}
			try
			{
				value_of(w.window.opener).should_be_object();
				failed = false;
			}
			catch(e)
			{
				callback.failed(e);
			}
			w.close();
			if (!failed)
			{
				callback.passed();
			}
		},1000);
	},
	test_window_opener_different_domain_as_async: function(callback)
	{
		var w = Titanium.UI.getCurrentWindow().createWindow('http://www.google.com/');
		w.open();
		setTimeout(function()
		{
			try
			{
				value_of(w.window.opener).should_be_null();
				callback.passed();
			}
			catch(e)
			{
				callback.failed(e);
			}
			w.close();
		},1500);
	},
	test_yahoo_white_black_window_as_async:function(callback)
	{
		// this is a small variation (for automation purposes) of the yahoo white/black test case for controlling
		// multiple titanium windows cross-domain and being able to inject variables/functions into
		// one or more cross domain ti windows and being able to do cross-window communication
		
		window.childWindows = {};
		var window_count = 0;
		
		function openPage(page_name,run_test) 
		{
			var w = Titanium.UI.createWindow("http://api.appcelerator.net/p/pages/unittest/"+page_name+".html");
			w.setWidth(320);
			w.setHeight(90);
			w.addEventListener(function(e)
			{
				if (e.getType()==e.PAGE_INITIALIZED)
				{
					// "give" child reference to our windows hash
					window.childWindows[page_name] = e.scope;
					e.scope.childWindows = window.childWindows;
				}
				else if (e.getType() == e.PAGE_LOADED)
				{
					window_count++;
					// wait to make sure we've got both windows open before running
					if (window_count == 2)
					{
						run_test();
					}
				}
			});
			w.open();
		}
		function runTest()
		{
			var passed = false;
			
			try
			{
				value_of(window.childWindows["black_page"]).should_be_object();
				value_of(window.childWindows["white_page"]).should_be_object();
				value_of(window.childWindows["black_page"].document).should_be_object();
				value_of(window.childWindows["white_page"].document).should_be_object();

				value_of(window.childWindows["black_page"].document.getElementById("button")).should_be_object();
				value_of(window.childWindows["white_page"].document.getElementById("button")).should_be_object();
				
				window.childWindows["black_page"].document.getElementById("button").click();
				window.childWindows["white_page"].document.getElementById("button").click();
				
				value_of(window.childWindows["black_page"].poke_result).should_be("The White Page");
				value_of(window.childWindows["white_page"].poke_result).should_be("The Black Page");

				passed = true;
			}
			catch(e)
			{
				callback.failed(e);
			}
			
			try
			{
				window.childWindows["black_page"].close();
			}
			catch(e)
			{
			}
			try
			{
				window.childWindows["white_page"].close();
			}
			catch(e)
			{
			}
			if (passed) callback.passed();
		}
		openPage("black_page",runTest);
		openPage("white_page",runTest);
	},
	issue32_unicode_in_title: function()
	{
		var win = Titanium.UI.getCurrentWindow().createWindow('http://en.wikipedia.org/wiki/The_Evil_Dead');
		win.setTitle("'添加新帐户");
		value_of(win.getTitle()).should_be("'添加新帐户");
	},
	TI224_test_mixcase_urls: function()
	{
		var url = 'http://en.wikipedia.org/wiki/The_Evil_Dead';
		var w = Titanium.UI.createWindow(url);
		w.open();
		
		value_of(w.getURL()).should_be(url);
		w.close();
	},
});
