describe("UI.Tray Tests",{
	test_ui_module_methods: function()
	{
		value_of(Titanium.UI.addTray).should_be_function();
		value_of(Titanium.UI.clearTray).should_be_function();
		value_of(Titanium.UI.Tray).should_not_be_null();
		
                "Tray.getHint": 0, 
                "Tray.getIcon": 0, 
                "Tray.getMenu": 0, 
                "Tray.remove": 0, 
                "Tray.setHint": 0, 
                "Tray.setIcon": 0, 
                "Tray.setMenu": 0, 
		
	},
	test_tray_object: function()
	{
	    var trayObj = Titanium.UI.addTray("app://logo_small.png", function(){
	        return;
	    });
	    
	    value_of(trayObj.getHint).should_be_function();
	    value_of(trayObj.getIcon).should_be_function();
	    value_of(trayObj.getMenu).should_be_function();
	    value_of(trayObj.remove).should_be_function();
	    value_of(trayObj.setHint).should_be_function();
	    value_of(trayObj.setIcon).should_be_function();
	    value_of(trayObj.setMenu).should_be_function();
	    
	    Titanium.UI.clearTray();
	}
});
