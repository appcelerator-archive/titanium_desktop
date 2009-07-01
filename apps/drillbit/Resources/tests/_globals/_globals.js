describe("ti global tests",
{
    test_globals: function()
    {
        // function to testout the global properties in Titanium.
        value_of(Titanium.platform).should_be_string();
        value_of(Titanium.version).should_be_string();
        
        Titanium.API.info("Titanium.platform = "+Titanium.platform);
        Titanium.API.info("Titanium.version = "+Titanium.version);
    }
});
