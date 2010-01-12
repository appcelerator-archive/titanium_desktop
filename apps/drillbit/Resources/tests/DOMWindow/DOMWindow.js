describe("UI Window Tests",{
	before_all: function()
	{
		var self = this;
		var open_window_timer = null;
		var open_window_assert = null;
		var open_window_test = null;

		Titanium.open_window_callback = function(windowIn)
		{
			clearTimeout(open_window_timer);
			try
			{
				open_window_assert.apply(self, [windowIn]);
				open_window_test.passed();

				// We now seem to be running in the global context of
				// the newly opened window. When the next test is executed
				// if this window is closed the settings allowing JavaScript
				// popups seems to be gone on the GTK+ port. The current work
				// around for this is to not close the window here.
				// windowIn.close();
			}
			catch (e)
			{
				open_window_test.failed(e);
			}
		}

		this.async_window_open = function(test, assertFn, args)
		{
			open_window_test = test;
			open_window_assert = assertFn;
			var w = window.open.apply(window, args);
			open_window_timer = setTimeout(function(){
				open_window_test.failed("Window open timed out.");
			}, 3000);
			return w;
		}
	},
	test_open_no_url_as_async: function(callback)
	{
		var w = window.open()
		setTimeout(function() {
			if (typeof(w) != "object" || typeof(w.Titanium) != 'object')
				callback.failed("Could not find Titanium object in window");
			else
				callback.passed();
			w.close();
		}, 200);
	},
	test_open_with_url_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium).should_be_object();
		}, ["a.html"]);
	},
	test_open_with_name_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
			value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_false();
		}, ["a.html", "a"]);
	},
	test_open_height_100_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
			value_of(w.Titanium.UI.getCurrentWindow().getHeight()).should_be(100);
		}, ["a.html","a","height=100"]);
	},
	test_open_width_121_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
			value_of(w.Titanium.UI.getCurrentWindow().getWidth()).should_be(121);
			value_of(w.Titanium.UI.getCurrentWindow().isResizable()).should_be_true();
		}, ["a.html","a","width=121"]);
	},
	// webkit and firefox both ignore the 'resizable' flag
	// see WebCore/page/WindowFeatures.cpp line 133
	test_open_left_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
			value_of(w.Titanium.UI.getCurrentWindow().getX()).should_be(125);
		}, ["a.html","a","left=125"]);
	},
	test_open_top_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
			value_of(w.Titanium.UI.getCurrentWindow().getY()).should_be(125);
		}, ["a.html","a","top=125"]);
	},
	test_open_multiple_props_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
			value_of(w.Titanium.UI.getCurrentWindow().getY()).should_be(125);
			value_of(w.Titanium.UI.getCurrentWindow().getX()).should_be(125);
			value_of(w.Titanium.UI.getCurrentWindow().getWidth()).should_be(121);
			value_of(w.Titanium.UI.getCurrentWindow().getHeight()).should_be(101);
			value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_false();
		}, ["a.html","a","left=125,top=125,width=121,height=101"]);
	},
	test_open_child_dom_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.result).should_be('Hello');
		}, ["a.html"]);
	},
	test_open_relative_app_url_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.result).should_be('Hello');
			value_of(w.a_value).should_be(42);
		}, ["app://rel.html"]);
	},
	test_window_focus_as_async: function(test)
	{
		// Open window1.
		w1 = Titanium.UI.createWindow("app://rel.html");
		w1.addEventListener(Titanium.PAGE_INITIALIZED, function() {

			//Open window2 so that window1 no longer has focus.
			w1.addEventListener(w1.FOCUSED, function() {test.passed();});
			w2 = Titanium.UI.createWindow("app://rel.html");
			w2.addEventListener(Titanium.PAGE_INITIALIZED, 
				function() { w1.domWindow.focus(); });
			w2.open();
		});
		w1.open();
		setTimeout(function() {test.failed("Didn't get focused message"); }, 4000);
	},
	test_window_unfocus_as_async: function(test)
	{
		// Open window1.
		w1 = Titanium.UI.createWindow("app://rel.html");
		w1.addEventListener(Titanium.UNFOCUSED, function() {test.passed();});
		w1.open();
		setTimeout(function() {
			w1.domWindow.blur();
		}, 1000);
		setTimeout(function() {test.failed("Didn't get unfocused message"); }, 4000);
	},
	//test_data_uri_non_base64_encoded_as_async: function(test)
	//{
	//	var path = Titanium.App.appURLToPath("app://rel.html");
	//	var html = Titanium.Filesystem.getFile(path).read();

	//	this.async_window_open(test, function(w)
	//	{
	//		value_of(w).should_be_object();
	//		value_of(w.Titanium).should_be_object();
	//		value_of(w.document.title).should_be("Hello");
	//		value_of(w.result).should_be('Hello');
	//	}, ["data:text/html;charset=utf-8,"+html]);
	//},
	//test_data_uri_base64_encoded_as_async: function(test)
	//{
	//	var path = Titanium.App.appURLToPath("app://rel.html");
	//	var html = Titanium.Filesystem.getFile(path).read();

	//	this.async_window_open(test, function(w)
	//	{
	//		value_of(w).should_be_object();
	//		value_of(w.Titanium).should_be_object();
	//		value_of(w.document.title).should_be("Hello");
	//		value_of(w.result).should_be('Hello');
	//	}, ["data:text/html;charset=utf-8;base64,"+Titanium.Codec.encodeBase64(html)]);
	//},
});
