describe("UI Window Tests",{
	test_window_open_no_url: function()
	{
		var w = window.open()
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_undefined();
		w.close();
	},
	test_window_open_with_url: function()
	{
		var w = window.open("a.html")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		w.close();
	},
	test_window_open_with_url_with_name: function()
	{
		var w = window.open("a.html","a")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_false();
		w.close();
	},
	test_window_open_with_url_with_fullscreen_as_string_true: function()
	{
		var w = window.open("a.html","a","fullscreen=yes")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_true();
		w.close();
	},
	test_window_open_with_url_with_fullscreen_as_string_false: function()
	{
		var w = window.open("a.html","a","fullscreen=no")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_false();
		w.close();
	},
	test_window_open_with_url_with_fullscreen_as_number: function()
	{
		var w = window.open("a.html","a","fullscreen=1")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_true();
		w.close();
	},
	test_window_open_with_url_with_fullscreen_as_number_zero: function()
	{
		var w = window.open("a.html","a","fullscreen=0")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_false();
		w.close();
	},
	test_window_open_with_url_with_height: function()
	{
		var w = window.open("a.html","a","height=100")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().getHeight()).should_be(100);
		w.close();
	},
	test_window_open_with_url_with_width: function()
	{
		var w = window.open("a.html","a","width=100")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().getWidth()).should_be(100);
		value_of(w.Titanium.UI.getCurrentWindow().isResizable()).should_be_true();
		w.close();
	},
	test_window_open_with_url_with_resizable_as_number: function()
	{
		var w = window.open("a.html","a","resizable=1")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().isResizable()).should_be_true();
		w.close();
	},
	test_window_open_with_url_with_resizable_as_number_zero: function()
	{
		var w = window.open("a.html","a","resizable=0")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().isResizable()).should_be_false();
		w.close();
	},
	test_window_open_with_url_with_resizable_as_boolean: function()
	{
		var w = window.open("a.html","a","resizable=true")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().isResizable()).should_be_true();
		w.close();
	},
	test_window_open_with_url_with_resizable_as_boolean_false: function()
	{
		var w = window.open("a.html","a","resizable=false")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().isResizable()).should_be_false();
		w.close();
	},
	test_window_open_with_url_with_left: function()
	{
		var w = window.open("a.html","a","left=2")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().getX()).should_be(2);
		w.close();
	},
	test_window_open_with_url_with_top: function()
	{
		var w = window.open("a.html","a","top=2")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().getY()).should_be(2);
		w.close();
	},
	test_window_open_with_url_with_multiple_props: function()
	{
		var w = window.open("a.html","a","left=2,top=2,fullscreen=0,width=10,height=10")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
		value_of(w.Titanium.UI.getCurrentWindow().getY()).should_be(2);
		value_of(w.Titanium.UI.getCurrentWindow().getX()).should_be(2);
		value_of(w.Titanium.UI.getCurrentWindow().getWidth()).should_be(10);
		value_of(w.Titanium.UI.getCurrentWindow().getHeight()).should_be(10);
		value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_false();
		w.close();
	},
	test_window_open_test_value_in_window_as_async: function(callback)
	{
		var w = window.open("a.html")
		value_of(w).should_be_object();
		value_of(w.Titanium).should_be_object();
		value_of(w.document.title).should_be("Hello");
		setTimeout(function()
		{
			try
			{
				value_of(w.result).should_be('Hello');
				callback.passed();
			}
			catch(e)
			{
				callback.failed(e);
			}
			w.close();
		},1000);
	},
});
