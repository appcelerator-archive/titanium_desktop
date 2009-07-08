describe("ti global tests",
{
	test_platform: function()
	{
		// function to testout the global properties in Titanium.
		value_of(Titanium.platform).should_be_string();
		// get the platform and version.
		value_of(Titanium.platform).should_be_one_of(['win32','osx','linux']);
		Titanium.API.info("Titanium.platform = "+Titanium.platform);
	},
	
	test_version: function()
	{
		value_of(Titanium.version).should_be_string();
		var arr = Titanium.version.split('.');
		value_of(arr.length).should_be(3);
		value_of(Number(arr[0])).should_be_number();
		value_of(Number(arr[1])).should_be_number();
		value_of(Number(arr[2])).should_be_number();

		Titanium.API.info("Titanium.version = "+Titanium.version);
	}
});
