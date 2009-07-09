describe("UI.Menu Tests (Active)",{
	test_basic_menu_append_and_getitemat_as_async: function(cb)
	{
		var w1 = Titanium.UI.getCurrentWindow();
		var w2 = Titanium.UI.createWindow({url: 'blah.html'});
		w2.open();

		var tui = Titanium.UI;
		var menu = tui.createMenu();
		value_of(menu.getLength()).should_be(0);
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
	}
});
