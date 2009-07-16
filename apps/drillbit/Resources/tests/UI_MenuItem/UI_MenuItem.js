describe("UI.MenuItem",{
	test_basic_methods: function()
	{
		var tui = Titanium.UI;
		value_of(tui.createMenuItem).should_be_function();
		value_of(tui.createCheckMenuItem).should_be_function();
		value_of(tui.createSeparatorMenuItem).should_be_function();

		var norm_type = 0;
		var check_type = 1;
		var sep_type = 2;

		var check_all = function(item, type) {
			value_of(item.isSeparator).should_be_function();
			value_of(item.isCheck).should_be_function();

			if (type == norm_type || type == check_type) {
				value_of(item.setLabel).should_be_function();
				value_of(item.getLabel).should_be_function();
				value_of(item.getSubmenu).should_be_function();
				value_of(item.setSubmenu).should_be_function();
				value_of(item.enable).should_be_function();
				value_of(item.disable).should_be_function();
				value_of(item.isEnabled).should_be_function();
				value_of(item.addItem).should_be_function();
				value_of(item.addSeparatorItem).should_be_function();
			} else {
				value_of(item.setLabel).should_be_undefined();
				value_of(item.getLabel).should_be_undefined();
				value_of(item.getSubmenu).should_be_undefined();
				value_of(item.setSubmenu).should_be_undefined();
				value_of(item.enable).should_be_undefined();
				value_of(item.disable).should_be_undefined();
				value_of(item.isEnabled).should_be_undefined();
				value_of(item.addItem).should_be_undefined();
				value_of(item.addSeparatorItem).should_be_undefined();
			}

			if (type == norm_type) {
				value_of(item.setIcon).should_be_function();
				value_of(item.getIcon).should_be_function();
			} else {
				value_of(item.setIcon).should_be_undefined();
				value_of(item.getIcon).should_be_undefined();
			}

			if (type == check_type) {
				value_of(item.getState).should_be_function();
				value_of(item.setState).should_be_function();
				value_of(item.isAutoCheck).should_be_function();
				value_of(item.setAutoCheck).should_be_function();
			} else {
				value_of(item.getState).should_be_undefined();
				value_of(item.setState).should_be_undefined();
				value_of(item.isAutoCheck).should_be_undefined();
				value_of(item.setAutoCheck).should_be_undefined();
			}
		}
		var menu = tui.createMenu();
		var item = tui.createMenuItem("blah1");
		var item2 = menu.addItem("blah2");
		var item3 = item.addItem("blah2");
		var check_item = tui.createCheckMenuItem("blah3");
		var check_item2 = menu.addCheckItem("blah4");
		var check_item3 = item.addCheckItem("blah5");
		var sep_item = tui.createSeparatorMenuItem("blah5");
		var sep_item2 = menu.addSeparatorItem("blah6");
		var sep_item3 = item.addSeparatorItem("blah7");

		check_all(item, norm_type);
		check_all(item2, norm_type);
		check_all(item3, norm_type);
		check_all(check_item, check_type);
		check_all(check_item2, check_type);
		check_all(check_item3, check_type);
		check_all(sep_item, sep_type);
		check_all(sep_item2, sep_type);
		check_all(sep_item3, sep_type);
	},
	test_setlabel: function()
	{
		var tui = Titanium.UI;
		value_of(tui.createMenuItem).should_be_
		var menu = tui.createMenu();
		var item = tui.createMenuItem("blah1");
		var item2 = menu.addItem("blah2");
		var item3 = item.addItem("blah3");
		var check_item = tui.createCheckMenuItem("blah4");
		var check_item2 = menu.addCheckItem("blah5");
		var check_item3 = item.addCheckItem("blah6");

		value_of(item.getLabel()).should_be("blah1");
		item.setLabel("booboo1");
		value_of(item.getLabel()).should_be("booboo1");
		value_of(item2.getLabel()).should_be("blah2");
		item2.setLabel("booboo2");
		value_of(item2.getLabel()).should_be("booboo2");
		value_of(item3.getLabel()).should_be("blah3");
		item3.setLabel("booboo3");
		value_of(item3.getLabel()).should_be("booboo3");
		value_of(check_item.getLabel()).should_be("blah4");
		check_item.setLabel("booboo1");
		value_of(check_item.getLabel()).should_be("booboo1");
		value_of(check_item2.getLabel()).should_be("blah5");
		check_item2.setLabel("booboo2");
		value_of(check_item2.getLabel()).should_be("booboo2");
		value_of(check_item3.getLabel()).should_be("blah6");
		check_item3.setLabel("booboo3");
		value_of(check_item3.getLabel()).should_be("booboo3");
	},
	test_enable_disable: function()
	{
		var tui = Titanium.UI;
		value_of(tui.createMenuItem).should_be_
		var menu = tui.createMenu();
		var item = tui.createMenuItem("blah1");
		var item2 = menu.addItem("blah2");
		var item3 = item.addItem("blah3");
		var check_item = tui.createCheckMenuItem("blah4");
		var check_item2 = menu.addCheckItem("blah5");
		var check_item3 = item.addCheckItem("blah6");

		var test_it = function(item) {
			value_of(item.isEnabled()).should_be_true();
			item.disable();
			value_of(item.isEnabled()).should_be_false();
			item.disable();
			value_of(item.isEnabled()).should_be_false();
			item.enable();
			value_of(item.isEnabled()).should_be_true();
			item.enable();
			value_of(item.isEnabled()).should_be_true();
		}

		test_it(item);
		test_it(item2);
		test_it(item3);
		test_it(check_item);
		test_it(check_item2);
		test_it(check_item3);
	},
	test_set_submenu: function()
	{
		var tui = Titanium.UI;
		var menu = tui.createMenu();
		var mitem = tui.createMenuItem("blah1");

		var item = tui.createMenuItem("blah1");
		var item2 = menu.addItem("blah2");
		var item3 = mitem.addItem("blah3");
		var check_item = tui.createCheckMenuItem("blah4");
		var check_item2 = menu.addCheckItem("blah5");
		var check_item3 = mitem.addCheckItem("blah6");

		var submenu1 = tui.createMenu();
		var submenu2 = tui.createMenu();

		var test_it = function(item) {
			value_of(item.getSubmenu()).should_be_null();
			item.setSubmenu(submenu1);
			value_of(item.getSubmenu().equals(submenu1)).should_be_true();
			value_of(item.getSubmenu().equals(submenu2)).should_be_false();
			item.setSubmenu(submenu2);
			value_of(item.getSubmenu().equals(submenu2)).should_be_true();
			value_of(item.getSubmenu().equals(submenu1)).should_be_false();
			item.setSubmenu(null);
			value_of(item.getSubmenu()).should_be_null();
		}
		test_it(item);
		test_it(item2);
		test_it(item3);
		test_it(check_item);
		test_it(check_item2);
		test_it(check_item3);
	},
	test_set_recursive_menu: function()
	{
		var menu = Titanium.UI.createMenu();
		menu.addItem("blah1");
		menu.addItem("blah2");
		menu.addItem("blah3");
		menu.addItem("blah4");
		var item1 = menu.addItem("blah5");
		var item2 = item1.addItem("blah6");
		var item3 = item2.addItem("blah6");
		var item4 = item3.addItem("blah6");
		var item5 = item4.addItem("blah6");
		var item6 = item5.addItem("blah6");

		var exc = true;
		try { item1.setSubmenu(menu);;exc = false } catch (e) { }
		value_of(exc).should_be(true);
		try { item6.setSubmenu(menu);;exc = false } catch (e) { }
		value_of(exc).should_be(true);

		var itemz = Titanium.UI.createMenuItem();
		itemz.setSubmenu(menu);
		try {
			item2.getSubmenu().appendItem(itemz);
			exc = false
		} catch (e) { }
		value_of(exc).should_be(true);

		try {
			item2.getSubmenu().insertItemAt(itemz, 0);
			exc = false
		} catch (e) { }
		value_of(exc).should_be(true);

	},
});
