describe("JSON tests",
{
	test_object: function()
	{
		value_of(Titanium.JSON).should_not_be_null();

		var obj = new Object();
		obj.intProp = 0;
		obj.strProp = "string";
		obj.func = function() {
			Titanium.API.info("inside a function");
		};

		value_of(obj).should_be_object();
		value_of(obj.intProp).should_be_number();
		value_of(obj.strProp).should_be_string();
		value_of(obj.func).should_be_function();

		// convert the object to a string.
		var jsonObj = Titanium.JSON.stringify(obj);

		value_of(jsonObj).should_be_string();

		Titanium.API.debug(jsonObj);
		var theObj = Titanium.JSON.parse(jsonObj);

		value_of(theObj).should_be_object();
		value_of(theObj.intProp).should_be_number();
		value_of(theObj.strProp).should_be_string();
		value_of(theObj.func).should_be_undefined();
	}
})
