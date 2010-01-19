describe("Update manager object tests",{
	test_object: function()
	{
		value_of(Titanium.UpdateManager).should_not_be_null();
		value_of(Titanium.UpdateManager.cancelMonitor).should_be_function();
		value_of(Titanium.UpdateManager.installAppUpdate).should_be_function();
		value_of(Titanium.UpdateManager.onupdate).should_be_null();
		value_of(Titanium.UpdateManager.startMonitor).should_be_function();
	},
	test_version_comparison: function()
	{
		var compare = Titanium.UpdateManager.compareVersions;
		value_of(compare("0.8.0", "0.8.0")).should_be(0);
		value_of(compare("0.8.0", "0.8.1")).should_be_less_than(0);
		value_of(compare("0.8.0", "0.8.0.0")).should_be_less_than(0);
		value_of(compare("0.8.0.0", "0.8.1")).should_be_less_than(0);
		value_of(compare("0.8.0.0", "0")).should_be_greater_than(0);
		value_of(compare("0", "0.8.0.0")).should_be_less_than(0);
		value_of(compare("0.8.a", "0.8.0")).should_be_less_than(0);

		value_of(compare("0", "0")).should_be(0);
		value_of(compare("0", "1")).should_be_less_than(0);
		value_of(compare("1", "0")).should_be_greater_than(0);
		value_of(compare("a", "b")).should_be_less_than(0)
		value_of(compare("c", "a")).should_be_greater_than(0);
		value_of(compare("0.a", "0.b")).should_be_less_than(0)
		value_of(compare("0.c", "0.a")).should_be_greater_than(0);
		value_of(compare("a.a", "a.b")).should_be_less_than(0)
		value_of(compare("0.0.0.0.0", "0.0.0.0.0")).should_be(0);
		value_of(compare("0.0.0.0.0.0", "0.0.0.0.0")).should_be_greater_than(0);

		value_of(compare("0.7.0", "0.8.0")).should_be_less_than(0)
		value_of(compare("0.7.0", "0.8.1")).should_be_less_than(0)
		value_of(compare("0.7.0", "0.8.0.0")).should_be_less_than(0)
		value_of(compare("0.7.0.0", "0.8.1")).should_be_less_than(0)
		value_of(compare("0.7.0.0", "0")).should_be_greater_than(0);
		value_of(compare("0", "0.8.0.0")).should_be_less_than(0)
		value_of(compare("0.7.a", "0.8.0")).should_be_less_than(0)

		value_of(compare("0.8.0", "0.7.0")).should_be_greater_than(0);
		value_of(compare("0.8.0", "0.7.1")).should_be_greater_than(0);
		value_of(compare("0.8.0", "0.7.0.0")).should_be_greater_than(0);
		value_of(compare("0.8.0.0", "0.7.1")).should_be_greater_than(0);
		value_of(compare("0.8.0.0", "0")).should_be_greater_than(0);
		value_of(compare("0", "0.7.0.0")).should_be_less_than(0)
		value_of(compare("0.8.a", "0.7.0")).should_be_greater_than(0);
	}

});
