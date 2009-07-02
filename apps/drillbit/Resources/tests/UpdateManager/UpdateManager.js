describe("Update manager object tests",{
	test_object:function()
	{
		value_of(Titanium.UpdateManager).should_not_be_null();
		value_of(Titanium.UpdateManager.cancelMonitor).should_be_function();
		value_of(Titanium.UpdateManager.installAppUpdate).should_be_function();
		value_of(Titanium.UpdateManager.onupdate).should_be_null();
		value_of(Titanium.UpdateManager.startMonitor).should_be_function();
	}
	// TODO - figure out how to test this object.
});