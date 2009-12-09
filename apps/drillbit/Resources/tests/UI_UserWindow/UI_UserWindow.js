describe("Window specific stuff",
{
	test_app_url_with_query_string_as_async: function(callback)
	{
		Titanium.page_loaded_callback = function()
		{
			callback.passed();
		}

		setTimeout(function()
		{
			callback.failed("Could not load app:// URL with query string.");
		}, 500);

		var w = Titanium.UI.createWindow("app://querystring.html?param=1");
		w.open();
	},
	test_app_url_with_fragment_as_async: function(callback)
	{
		Titanium.page_loaded_callback = function()
		{
			callback.passed();
		}

		setTimeout(function()
		{
			callback.failed("Could not load app:// URL with URL fragment.");
		}, 500);

		var w = Titanium.UI.createWindow("app://querystring.html#fooness");
		w.open();
	},
	test_app_url_with_fragment_and_query_string_as_async: function(callback)
	{
		Titanium.page_loaded_callback = function()
		{
			callback.passed();
		}

		setTimeout(function()
		{
			callback.failed("Could not load app:// URL with URL fragment and query string.");
		}, 500);

		var w = Titanium.UI.createWindow("app://querystring.html?param=1#fooness");
		w.open();
	},
	test_relative_url_with_query_string_as_async: function(callback)
	{
		Titanium.page_loaded_callback = function()
		{
			callback.passed();
		}

		setTimeout(function()
		{
			callback.failed("Could not load app:// URL with URL fragment and query string.");
		}, 500);

		var w = Titanium.UI.createWindow("app://querystring2.html");
		w.open();
	},
	test_tool_window_as_async: function(callback)
	{
		var w = Titanium.UI.createWindow();
		var w2 = Titanium.UI.createWindow();
		var w3 = Titanium.UI.getCurrentWindow().createWindow({toolWindow:true});
		w.setToolWindow(true);

		var verify = function()
		{
			try
			{
				value_of(w.isToolWindow()).should_be_true();
				value_of(w2.isToolWindow()).should_be_false();
				value_of(w3.isToolWindow()).should_be_true();
				callback.passed();
			}
			catch(e)
			{
				callback.failed(e);
			}
		}

		verify();
		w.open();
		w2.open();
		w3.open();
		verify();
		setTimeout(function()
		{
			verify();
			w.close();
			w2.close();
			w3.close();
		},500);
	},
	test_tool_window_reverse_as_async: function(callback)
	{
		var w = Titanium.UI.createWindow();
		w.setToolWindow(true);
		w.setToolWindow(false);

		var verify = function()
		{
			try
			{
				value_of(w.isToolWindow()).should_be_false();
				callback.passed();
			}
			catch(e)
			{
				callback.failed(e);
			}
		}

		verify();
		w.open();
		verify();

		setTimeout(function()
		{
			verify();
			w.close();
		},500);
	},
});
