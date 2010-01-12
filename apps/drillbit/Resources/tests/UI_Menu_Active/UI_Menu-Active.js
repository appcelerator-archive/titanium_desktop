describe("UI.Menu (Active)",{
	test_basic_menu_append_and_getitemat_as_async: function(cb)
	{
		var w1 = Titanium.UI.getCurrentWindow();
		var w2 = Titanium.UI.createWindow({url: 'blah.html'});
		w2.open();

		var tui = Titanium.UI;
		var menu = tui.createMenu();
		w1.setMenu(menu);
		w2.setMenu(menu);

		setTimeout(function () {
			var item1 = tui.createMenuItem('blah1');
			var item2 = tui.createMenuItem('blah2');
			var item3 = tui.createMenuItem('blah3');
			menu.appendItem(item1);
			menu.appendItem(item2)
			menu.appendItem(item3);

			if (menu.getLength() != 3 ||
				!menu.getItemAt(0).equals(item1) ||
				!menu.getItemAt(1).equals(item2) ||
				!menu.getItemAt(2).equals(item3))
				cb.failed("Length incorrect")

			var exc = true;
			try { var a = menu.getItemAt(-1); exc = false } catch (e) { }
			try { var a = menu.getItemAt(3); exc = false } catch (e) { }
			try { var a = menu.getItemAt(30); exc = false } catch (e) { }
			try { var a = menu.getItemAt(-30); exc = false } catch (e) { }
			if (!exc)
				cb.failed("no exception");

			cb.passed();
		}, 200);
	},
	test_active_removeitem_at_as_async: function(cb)
	{
		var w1 = Titanium.UI.getCurrentWindow();
		var w2 = Titanium.UI.createWindow({url: 'blah.html'});
		w2.open();

		var tui = Titanium.UI;
		var menu = tui.createMenu();
		w1.setMenu(menu);
		w2.setMenu(menu);

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

		setTimeout(function () {
			menu.removeItemAt(0);
			if (menu.getLength() != 4 ||
				!menu.getItemAt(0).equals(item2) ||
				!menu.getItemAt(1).equals(item3) ||
				!menu.getItemAt(2).equals(item4) ||
				!menu.getItemAt(3).equals(item5))
				cb.failed("Part 1 Failed");

			menu.removeItemAt(1);
			if (menu.getLength() != 3 ||
				!menu.getItemAt(0).equals(item2) ||
				!menu.getItemAt(1).equals(item4) ||
				!menu.getItemAt(2).equals(item5))
				cb.failed("Part 2 failed");

			menu.appendItem(item1);
			menu.removeItemAt(3);
			if (menu.getLength() != 3 ||
				!menu.getItemAt(0).equals(item2) ||
				!menu.getItemAt(1).equals(item4) ||
				!menu.getItemAt(2).equals(item5))
				cb.failed("Part 3 failed");
			cb.passed();
		}, 200);
	}
});
