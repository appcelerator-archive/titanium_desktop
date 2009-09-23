describe("UI.Clipboard",{
	test_clipboard_text_data_set_twice: function()
	{
		Titanium.UI.Clipboard.setData("text/plain", "blahblah");
		var data = Titanium.UI.Clipboard.getData("text/plain");
		value_of(data).should_be("blahblah");

		Titanium.UI.Clipboard.setData("text/plain", "blahblah");
		var data = Titanium.UI.Clipboard.getData("text/plain");
		value_of(data).should_be("blahblah");
	},
	test_clipboard_methods: function()
	{
		value_of(Titanium.UI.Clipboard).should_be_object();
		value_of(Titanium.UI.Clipboard.setData).should_be_function();
		value_of(Titanium.UI.Clipboard.getData).should_be_function();
		value_of(Titanium.UI.Clipboard.clearData).should_be_function();
		value_of(Titanium.UI.Clipboard.setText).should_be_function();
		value_of(Titanium.UI.Clipboard.getText).should_be_function();
	},
	test_clipboard_text_data: function()
	{
		Titanium.UI.Clipboard.setData("text/plain", "blahblah");
		var data = Titanium.UI.Clipboard.getData("text/plain");
		value_of(data).should_be("blahblah");

		Titanium.UI.Clipboard.setData("text/plain", "");
		value_of(Titanium.UI.Clipboard.hasText()).should_be_false();
		var data = Titanium.UI.Clipboard.getData("text/plain");
		value_of(data).should_be("");

		Titanium.UI.Clipboard.setData("text/plain", "crazy utf8 ‽‽‽ ⸮⸮⸮ woohoo");
		data = Titanium.UI.Clipboard.getData("text/plain");
		value_of(data).should_be("crazy utf8 ‽‽‽ ⸮⸮⸮ woohoo");
	},
	test_clipboard_text: function()
	{
		Titanium.UI.Clipboard.setText("blahblah");
		var data = Titanium.UI.Clipboard.getText();
		value_of(data).should_be("blahblah");

		Titanium.UI.Clipboard.setText("");
		value_of(Titanium.UI.Clipboard.hasText()).should_be_false();
		data = Titanium.UI.Clipboard.getText();
		value_of(data).should_be("");

		Titanium.UI.Clipboard.setText("crazy utf8 ‽‽‽ ⸮⸮⸮ woohoo");
		data = Titanium.UI.Clipboard.getText();
		value_of(data).should_be("crazy utf8 ‽‽‽ ⸮⸮⸮ woohoo");
	},
	text_clipboard_clear_data: function()
	{
		Titanium.UI.Clipboard.setText("blahblah");
		Titanium.UI.Clipboard.setData("text/plain", "blahblah");
		Titanium.UI.Clipboard.clearData("text/plain");

		var data = Titanium.UI.Clipboard.getText();
		value_of(data).should_be("");

		data = Titanium.UI.Clipboard.getData("text/plain");
		value_of(data).should_be("");
	},
	text_clipboard_clear_text: function()
	{
		Titanium.UI.Clipboard.setText("blahblah");
		Titanium.UI.Clipboard.setData("text/plain", "blahblah");

		Titanium.UI.Clipboard.clearText();

		var data = Titanium.UI.Clipboard.getText();
		value_of(data).should_be("");

		data = Titanium.UI.Clipboard.getData("text/plain");
		value_of(data).should_be("");

		// TODO: This should eventually set other data types on the
		// clipboard and ensure that they are *not* cleared.
	},
	text_clipboard_has_text: function()
	{
		Titanium.UI.Clipboard.setText("blahblah");
		value_of(Titanium.UI.Clipboard.hasText()).should_be_true();

		Titanium.UI.Clipboard.clearData("text/plain");
		value_of(Titanium.UI.Clipboard.hasText()).should_be_false();

		Titanium.UI.Clipboard.setText("blahblah");
		value_of(Titanium.UI.Clipboard.hasText()).should_be_true();

		Titanium.UI.Clipboard.clearData("text/plain");
		value_of(Titanium.UI.Clipboard.hasText()).should_be_false();

		Titanium.UI.Clipboard.setText("");
		value_of(Titanium.UI.Clipboard.hasText()).should_be_false();
	},
	test_clipboard_urilist_data: function()
	{
		var uri1 = Titanium.Filesystem.getApplicationDirectory().toURL();
		var uri2 = Titanium.Filesystem.getResourcesDirectory().toURL();
		var uri3 = Titanium.Filesystem.getDesktopDirectory().toURL();
		var uristring = uri1 + "\n" + uri2 + "\n" + uri3;
		Titanium.UI.Clipboard.setData("text/uri-list", uristring);
		var data = Titanium.UI.Clipboard.getData("text/uri-list");
		value_of(data).should_be_array();
		value_of(data.length).should_be(3);
		value_of(data[0].indexOf(uri1)).should_be(0); // A trailing slash may have been added
		value_of(data[1].indexOf(uri2)).should_be(0); // A trailing slash may have been added
		value_of(data[2].indexOf(uri3)).should_be(0); // A trailing slash may have been added

		Titanium.UI.Clipboard.setData("text/uri-list", null);
		value_of(Titanium.UI.Clipboard.hasData("text/uri-list")).should_be_false();
		var data = Titanium.UI.Clipboard.getData("text/uri-list");
		value_of(data).should_be_array();
		value_of(data.length).should_be(0);

		Titanium.UI.Clipboard.setData("text/uri-list", [uri1, uri2, uri3]);
		var data = Titanium.UI.Clipboard.getData("text/uri-list");
		value_of(data).should_be_array();
		value_of(data.length).should_be(3);
		value_of(data[0].indexOf(uri1)).should_be(0); // A trailing slash may have been added
		value_of(data[1].indexOf(uri2)).should_be(0); // A trailing slash may have been added
		value_of(data[2].indexOf(uri3)).should_be(0); // A trailing slash may have been added

		Titanium.UI.Clipboard.setData("text/uri-list", null);
		value_of(Titanium.UI.Clipboard.hasData("text/uri-list")).should_be_false();
		var data = Titanium.UI.Clipboard.getData("text/uri-list");
		value_of(data).should_be_array();
		value_of(data.length).should_be(0);
	},
	text_clipboard_clear_uri_list: function()
	{
		var uri1 = Titanium.Filesystem.getApplicationDirectory().toURL();
		var uri2 = Titanium.Filesystem.getResourcesDirectory().toURL();
		var uri3 = Titanium.Filesystem.getDesktopDirectory().toURL();
		var uristring = uri1 + "\n" + uri2 + "\n" + uri3;
		Titanium.UI.Clipboard.setData("text/uri-list", uristring);
		var data = Titanium.UI.Clipboard.getData("text/uri-list");
		value_of(data).should_be_array();
		value_of(data.length).should_be(3);
		value_of(data[0].indexOf(uri1)).should_be(0); // A trailing slash may have been added
		value_of(data[1].indexOf(uri2)).should_be(0); // A trailing slash may have been added
		value_of(data[2].indexOf(uri3)).should_be(0); // A trailing slash may have been added

		Titanium.UI.Clipboard.clearData("text/uri-list");
		value_of(Titanium.UI.Clipboard.hasData("text/uri-list")).should_be(false);
	},
	test_clipboard_url_data: function()
	{
		Titanium.UI.Clipboard.setData("url", "http://www.google.com");
		var data = Titanium.UI.Clipboard.getData("url");
		value_of(data).should_be("http://www.google.com");
		value_of(Titanium.UI.Clipboard.hasData("url")).should_be(true);

		Titanium.UI.Clipboard.setData("url", "http://www.yahoo.com");
		data = Titanium.UI.Clipboard.getData("url");
		value_of(data).should_be("http://www.yahoo.com");
		value_of(Titanium.UI.Clipboard.hasData("url")).should_be(true);

		Titanium.UI.Clipboard.setData("url", null);
		value_of(Titanium.UI.Clipboard.hasData("url")).should_be_false();
		data = Titanium.UI.Clipboard.getData("url");
		value_of(data).should_be("");

		Titanium.UI.Clipboard.setData("url", ["http://www.google.com", "http://www.yahoo.com"]);
		data = Titanium.UI.Clipboard.getData("url");
		value_of(data).should_be("http://www.google.com");
		value_of(Titanium.UI.Clipboard.hasData("url")).should_be_true();

		Titanium.UI.Clipboard.setData("url", "");
		value_of(Titanium.UI.Clipboard.hasData("url")).should_be_false();
		data = Titanium.UI.Clipboard.getData("url");
		value_of(data).should_be("");
	},
	text_clipboard_clear_url_list: function()
	{
		Titanium.UI.Clipboard.setData("url", "http://www.yahoo.com");
		var data = Titanium.UI.Clipboard.getData("url");
		value_of(data).should_be("http://www.yahoo.com");

		Titanium.UI.Clipboard.clearData("url");
		value_of(Titanium.UI.Clipboard.hasData("url")).should_be(false);
	},
});
