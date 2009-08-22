describe("php tests",
{
	test_bind_types: function()
	{
		value_of(window.bind_types).should_be_function();
		
		var obj = {};
		window.bind_types(obj);
		
		value_of(obj.number).should_be(1);
		value_of(obj.str).should_be("string");
		value_of(obj.list).should_match_array([1,2,3,4]);
		value_of(obj.hash).should_be_array();
		value_of(obj.hash.a).should_be('b');
		value_of(obj.object).should_be_object();
		value_of(obj.object.testmethod).should_be_function();
	}
});