describe("UI.Clipboard",{
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
});
