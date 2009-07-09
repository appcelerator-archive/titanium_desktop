describe("test ti.Analytics",{
	test_analytics_properties: function()
	{
		value_of(Titanium.Analytics).should_be_object();
		value_of(Titanium.Analytics.addEvent).should_be_function();
	},
});
