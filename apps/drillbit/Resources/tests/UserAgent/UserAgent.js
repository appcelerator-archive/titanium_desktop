describe("Contents of user agent",
{
	validate_name_and_version: function()
	{
		value_of(Titanium.userAgent).should_contain('Titanium/');
		value_of(Titanium.userAgent).should_contain('Titanium/'+Titanium.version);

		// for now, we're going to simulate Safari -- this addresses TI-303
		value_of(Titanium.userAgent).should_contain('Safari');

	}
});
