describe("UI.Menu",{
	test_basic_methods: function()
	{
		var tui = Titanium.UI;
		value_of(tui.createMenu).should_be_function();
		value_of(tui.createMenuItem).should_be_function();
		value_of(tui.createCheckMenuItem).should_be_function();
		value_of(tui.createSeparatorMenuItem).should_be_function();
		value_of(tui.setMenu).should_be_function();
		value_of(tui.setContextMenu).should_be_function();
		if (Titanium.platform == "osx") {
			value_of(tui.setDockMenu).should_be_function();
		}

		var w = Titanium.UI.getCurrentWindow();
		value_of(w.setMenu).should_be_function();
		value_of(w.setContextMenu).should_be_function();
	},
	test_basic_menu_methods: function()
	{
		var menu = Titanium.UI.createMenu();
		value_of(menu.getLength).should_be_function();
		value_of(menu.getItemAt).should_be_function();
		value_of(menu.insertItemAt).should_be_function();
		value_of(menu.removeItemAt).should_be_function();
		value_of(menu.clear).should_be_function();
		value_of(menu.addItem).should_be_function();
		value_of(menu.addSeparatorItem).should_be_function();
		value_of(menu.addCheckItem).should_be_function();
	},
	test_basic_menu_item: function()
	{
		var menuItem = Titanium.UI.createMenuItem("Blahblah");
		value_of(menuItem.getLabel).should_be_function();
		value_of(menuItem.setLabel).should_be_function();
		value_of(menuItem.getIcon).should_be_function();
		value_of(menuItem.setIcon).should_be_function();
		value_of(menuItem.addItem).should_be_function();
		value_of(menuItem.addSeparatorItem).should_be_function();
		value_of(menuItem.addCheckItem).should_be_function();
		value_of(menuItem.getLabel()).should_be("Blahblah");
	},
	test_basic_menu_append_and_getitemat: function()
	{
		var tui = Titanium.UI;
		var menu = tui.createMenu();
		value_of(menu.getLength()).should_be(0);

		var item1 = tui.createMenuItem('blah1');
		var item2 = tui.createMenuItem('blah2');
		var item3 = tui.createMenuItem('blah3');
		menu.appendItem(item1);
		menu.appendItem(item2)
		menu.appendItem(item3);

		value_of(menu.getLength()).should_be(3);
		value_of(menu.getItemAt(0).equals(item1)).should_be_true();
		value_of(menu.getItemAt(1).equals(item2)).should_be_true();
		value_of(menu.getItemAt(2).equals(item3)).should_be_true();

		var exc = true;
		try { var a = menu.getItemAt(-1); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { var a = menu.getItemAt(3); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { var a = menu.getItemAt(30); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { var a = menu.getItemAt(-30); exc = false } catch (e) { }
		value_of(exc).should_be(true);
	},
	test_menu_removeitemat: function()
	{
		var tui = Titanium.UI;
		var menu = tui.createMenu();
		value_of(menu.getLength()).should_be(0);

		var item1 = tui.createMenuItem('blah1');
		var item2 = tui.createMenuItem('blah2');
		var item3 = tui.createMenuItem('blah3');
		var item4 = tui.createMenuItem('blah4');
		var item5 = tui.createMenuItem('blah5');
		menu.appendItem(item1);
		menu.appendItem(item2)
		menu.appendItem(item3);
		menu.appendItem(item4);
		menu.appendItem(item5);

		menu.removeItemAt(0);
		value_of(menu.getLength()).should_be(4);
		value_of(menu.getItemAt(0).equals(item2)).should_be_true();
		value_of(menu.getItemAt(1).equals(item3)).should_be_true();
		value_of(menu.getItemAt(2).equals(item4)).should_be_true();
		value_of(menu.getItemAt(3).equals(item5)).should_be_true();

		menu.removeItemAt(1);
		value_of(menu.getLength()).should_be(3);
		value_of(menu.getItemAt(0).equals(item2)).should_be_true();
		value_of(menu.getItemAt(1).equals(item4)).should_be_true();
		value_of(menu.getItemAt(2).equals(item5)).should_be_true();

		menu.appendItem(item1);
		menu.removeItemAt(3);
		value_of(menu.getLength()).should_be(3);
		value_of(menu.getItemAt(0).equals(item2)).should_be_true();
		value_of(menu.getItemAt(1).equals(item4)).should_be_true();
		value_of(menu.getItemAt(2).equals(item5)).should_be_true();
	},
	test_invalid_removeitem_at: function()
	{
		var tui = Titanium.UI;
		var menu = tui.createMenu();
		value_of(menu.getLength()).should_be(0);

		var exc = true;
		try { menu.removeItemAt(-1); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.removeItemAt(-100); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.removeItemAt(0); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.removeItemAt(1); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.removeItemAt(10); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.removeItemAt(100); exc = false } catch (e) { }
		value_of(exc).should_be(true);

		var item1 = menu.addItem('blah1');
		var item2 = menu.addItem('blah2');

		try { menu.removeItemAt(-1); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.removeItemAt(-100); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.removeItemAt(10); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.removeItemAt(100); exc = false } catch (e) { }
		value_of(exc).should_be(true);

		try { menu.removeItemAt(1); exc = false } catch (e) { }
		value_of(exc).should_be(false);
	},
	test_menu_insertitemat: function()
	{
		var tui = Titanium.UI;
		var menu = tui.createMenu();
		value_of(menu.getLength()).should_be(0);

		var item1 = tui.createMenuItem('blah1');
		var item2 = tui.createMenuItem('blah2');
		var item3 = tui.createMenuItem('blah2');
		menu.appendItem(item1);
		menu.appendItem(item2)

		menu.insertItemAt(item3, 0);
		value_of(menu.getLength()).should_be(3);
		value_of(menu.getItemAt(0).equals(item3)).should_be_true();
		value_of(menu.getItemAt(1).equals(item1)).should_be_true();
		value_of(menu.getItemAt(2).equals(item2)).should_be_true();

		menu.insertItemAt(item3, 2);
		value_of(menu.getLength()).should_be(4);
		value_of(menu.getItemAt(0).equals(item3)).should_be_true();
		value_of(menu.getItemAt(1).equals(item1)).should_be_true();
		value_of(menu.getItemAt(2).equals(item3)).should_be_true();
		value_of(menu.getItemAt(3).equals(item2)).should_be_true();

		menu.insertItemAt(item3, 4);
		value_of(menu.getLength()).should_be(5);
		value_of(menu.getItemAt(0).equals(item3)).should_be_true();
		value_of(menu.getItemAt(1).equals(item1)).should_be_true();
		value_of(menu.getItemAt(2).equals(item3)).should_be_true();
		value_of(menu.getItemAt(3).equals(item2)).should_be_true();
		value_of(menu.getItemAt(4).equals(item3)).should_be_true();
	},
	test_invalid_insertitemat: function ()
	{
		var tui = Titanium.UI;
		var menu = tui.createMenu();
		value_of(menu.getLength()).should_be(0);

		var item1 = tui.createMenuItem('blah1');
		var item2 = tui.createMenuItem('blah2');
		var item3 = tui.createMenuItem('blah2');
		menu.appendItem(item1);
		menu.appendItem(item2)

		var exc = true;
		try { menu.insertItemAt(-1, item2); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.insertItemAt(-100, item2); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.insertItemAt(100, item2); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.insertItemAt(10, item2); exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { menu.insertItemAt(item2, 1); exc = false } catch (e) { }
		value_of(exc).should_be(false);
	},
	test_basic_menu_add_separator: function()
	{
		var tui = Titanium.UI;
		var menu = tui.createMenu();
		value_of(menu.getLength()).should_be(0);

		var item1 = menu.addSeparatorItem();
		var item2 = tui.createMenuItem('blah2');
		var item3 = tui.createMenuItem('blah3');
		menu.appendItem(item2)
		menu.appendItem(item3);

		value_of(menu.getLength()).should_be(3);
		value_of(menu.getItemAt(0).equals(item1)).should_be_true();
		value_of(menu.getItemAt(1).equals(item2)).should_be_true();
		value_of(menu.getItemAt(2).equals(item3)).should_be_true();

		value_of(item1.isSeparator()).should_be(true);
		value_of(item1.isCheck()).should_be(false);
	},
	test_basic_menu_add_item: function()
	{
		var tui = Titanium.UI;
		var menu = tui.createMenu();
		value_of(menu.getLength()).should_be(0);

		var cb = function() {alert('hey');};
		var item1 = menu.addItem("MyMyLabel", cb, "app://blah.png");
		var item2 = tui.createMenuItem('blah2');
		var item3 = tui.createMenuItem('blah3');
		menu.appendItem(item2)
		menu.appendItem(item3);

		value_of(menu.getLength()).should_be(3);
		value_of(menu.getItemAt(0).equals(item1)).should_be_true();
		value_of(menu.getItemAt(1).equals(item2)).should_be_true();
		value_of(menu.getItemAt(2).equals(item3)).should_be_true();

		value_of(item1.isSeparator()).should_be(false);
		value_of(item1.isCheck()).should_be(false);
		value_of(item1.getLabel()).should_be("MyMyLabel");
		value_of(item1.getIcon()).should_be("app://blah.png");
	},
	test_basic_menu_add_check: function()
	{
		var tui = Titanium.UI;
		var menu = tui.createMenu();
		value_of(menu.getLength()).should_be(0);

		var cb = function() {alert('hey');};
		var item1 = menu.addCheckItem("MyMyLabel");
		var item2 = tui.createMenuItem('blah2');
		var item3 = tui.createMenuItem('blah3');
		menu.appendItem(item2)
		menu.appendItem(item3);

		value_of(menu.getLength()).should_be(3);
		value_of(menu.getItemAt(0).equals(item1)).should_be_true();
		value_of(menu.getItemAt(1).equals(item2)).should_be_true();
		value_of(menu.getItemAt(2).equals(item3)).should_be_true();

		value_of(item1.isSeparator()).should_be(false);
		value_of(item1.isCheck()).should_be(true);
		value_of(item1.getLabel()).should_be("MyMyLabel");
	}
});
