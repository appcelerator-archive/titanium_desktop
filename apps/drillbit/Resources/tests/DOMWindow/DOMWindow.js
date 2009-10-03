describe("UI Window Tests",{
	before_all: function()
	{
		var self = this;
		this.async_window_open = function(test, assertFn, args)
		{
			var w = window.open.apply(window, args);
			value_of(w).should_be_object();
			Titanium.API.addEventListener(Titanium.PAGE_INITIALIZED, function(event)
			{
				if (!event.hasTitaniumObject)
					return;

				w.callback = function()
				{
					clearTimeout(timer);
					try
					{
						assertFn.apply(self, [w]);
						w.close();
						test.passed();
					}
					catch (e)
					{
						test.failed(e);
					}
				};	
			});
			timer = setTimeout(function(){
				test.failed("open url timed out");
			}, 3000);
		};
	},
	test_open_no_url_as_async: function(callback)
	{
		var w = window.open()
		setTimeout(function() {
			Titanium.API.debug(w.Titanium);
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
	test_open_fullscreen_yes_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
			if (Titanium.platform != 'linux')
			{
				value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_true();
			}
		}, ["a.html","a","fullscreen=yes"]);
	},
	test_open_fullscreen_no_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();

			if (Titanium.platform != 'linux')
			{
				value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_false();
			}
		}, ["a.html","a","fullscreen=no"]);
	},
	test_open_fullscreen_1_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
			if (Titanium.platform != 'linux')
			{
				value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_true();
			}
		}, ["a.html","a","fullscreen=1"]);
	},
	test_open_fullscreen_0_as_async: function(test)
	{
		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.Titanium.UI.getCurrentWindow()).should_be_object();
			if (Titanium.platform != 'linux')
			{
				value_of(w.Titanium.UI.getCurrentWindow().isFullscreen()).should_be_false();
			}
		}, ["a.html","a","fullscreen=0"]);
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
		}, ["a.html","a","left=125,top=125,fullscreen=0,width=121,height=101"]);
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
	test_data_uri_non_base64_encoded_as_async: function(test)
	{
		var path = Titanium.App.appURLToPath("app://rel.html");
		var html = Titanium.Filesystem.getFile(path).read();

		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.result).should_be('Hello');
		}, ["data:text/html;charset=utf-8,"+html]);
	},
	test_data_uri_base64_encoded_as_async: function(test)
	{
		var path = Titanium.App.appURLToPath("app://rel.html");
		var html = Titanium.Filesystem.getFile(path).read();

		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.result).should_be('Hello');
		}, ["data:text/html;charset=utf-8;base64,"+Titanium.Codec.encodeBase64(html)]);
	},
	test_data_uri_with_html_parameter_as_async: function(test)
	{
		var path = Titanium.App.appURLToPath("app://rel.html");
		var html = Titanium.Filesystem.getFile(path).read();

		this.async_window_open(test, function(w)
		{
			value_of(w).should_be_object();
			value_of(w.Titanium).should_be_object();
			value_of(w.document.title).should_be("Hello");
			value_of(w.result).should_be('Hello');
		}, [{html:html}]);
	}
});
