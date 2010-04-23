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
		}, 500);
	},
	test_set_contents_as_async: function(callback)
	{
		var w = Titanium.UI.createWindow();
		var one = '<html><body><div id="content">Hello.</div>' +
			'<script>Titanium.WindowContentsTest.phaseOne(document.getElementById("content").innerText);</script>' +
			'</body></html>';
		var two = '<html><body><div id="content">Hello.</div>' +
			'<script>Titanium.WindowContentsTest.success();</script>' +
			'</body></html>';

		Titanium.WindowContentsTest = {
			phaseOne: function(text)
			{
				if (text != "Hello.")
					callback.failed("Incorrect text passed: " + text);
				w.setContents(two);
			},
			success: function()
			{
				callback.passed();
			}
		};

		w.setContents(one);
		w.open();

		setTimeout(function()
		{
			callback.failed("Test timed out");
			w.close();
		}, 5000);
	},
	test_set_self_contents_as_async: function(callback)
	{
		Titanium.WindowContentsTest = {
			success: function() { callback.passed(); },
			nextPage: "<html><body><script>Titanium.WindowContentsTest.success();</script></body></html>"
		};
		var w = Titanium.UI.createWindow();
		var contents = '<html><body><div id="content">Hello.</div>' +
			'<script>\n' +
			'Titanium.UI.currentWindow.setContents(Titanium.WindowContentsTest.nextPage);\n' +
			'</script></body></html>';
		w.setContents(contents);
		w.open();

		setTimeout(function()
		{
			callback.failed("Test timed out");
			w.close();
		}, 5000);
	},
	test_set_contents_relative_urls_as_async: function(callback)
	{
		Titanium.WindowContentsTest = {
			success: function(text) {
				if (text == "foo")
					callback.passed();
				else
					callback.failed("Incorrect text passed: " + text);
			}
		};

		var w = Titanium.UI.createWindow();
		w.setContents(
			'<html><head>' +
			'<meta http-equiv="refresh" content="1;URL=relative.html">' + 
			'</head><body>redirect</body></html>',
			'app://subdir/page.html');
		w.open();
		setTimeout(function()
		{
			callback.failed("Test timed out");
			w.close();
		}, 5000);
	},
	test_set_window_height_with_menu_as_async: function(callback)
	{

		var w = Titanium.UI.createWindow();

		var checkHeight = function()
		{
			if (w.height != 500)
				callback.failed("Height was supposed to be 500, but was " + w.height);
			else
				callback.passed();
		};

		var changeHeight = function()
		{
			w.setHeight(500);
			setTimeout(checkHeight, 200);
		};

		Titanium.WindowCallback = function()
		{
			var menu = Titanium.UI.createMenu();
			menu.addItem("Foo1");
			menu.getItemAt(0).addItem("Foo");
			w.setMenu(menu);
			setTimeout(changeHeight, 200);
		};

		var contents = '<html><body><div id="content">Hello.</div>' +
			'<script>\nTitanium.WindowCallback();\n' +
			'</script></body></html>';
		w.setContents(contents);
		w.open();

		setTimeout(function()
		{
			callback.failed("Test timed out");
			w.close();
		}, 5000);
	},
});
