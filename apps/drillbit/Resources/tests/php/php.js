describe("php tests",
{
	test_simple_context: function()
	{
		value_of(window.a).should_be("1234");
	}
	
	/*test_bind_types: function()
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
		
		obj.object.testmethod();
		value_of(obj.object.value).should_be(100);
	},
	
	test_inline: function()
	{
		value_of(window.inline_test_result).should_be('A');
	},
	
	test_external_file: function()
	{
		value_of(window.external_test_result).should_be('A');
	},
	
	test_window_global_from_php: function()
	{
		// test to make sure that we can access a function defined
		// in normal javascript block from within ruby 
		value_of(window.test_window_global_result).should_be('you suck ass');
	},
	
	test_document_title_from_php: function()
	{
		value_of(window.test_document_title_result).should_be('I love Matz');
	},
	test_js_types: function()
	{
		value_of(test_js_type_string('1')).should_be_true();
		value_of(test_js_type_float(123)).should_be_true();
		value_of(test_js_type_float(0)).should_be_true();
		value_of(test_js_type_float(10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)).should_be_true();
		value_of(test_js_type_float(3.14)).should_be_true();
		value_of(test_js_type_object(function() { }, 'PHPKMethod')).should_be_true();
		value_of(test_js_type_object([1,2,3], 'PHPKList')).should_be_true();
		value_of(test_js_klist_elements([1,2,3])).should_be_true();
		value_of(test_js_type_object({'a1':'sauce'}, 'PHPKObject')).should_be_true();
		value_of(test_js_type_object({}, 'PHPKObject')).should_be_true();
		value_of(test_js_type_null(null)).should_be_true();
		value_of(test_js_type_null(undefined)).should_be_true();
		value_of(test_js_type_false_bool(false)).should_be_true();
		value_of(test_js_type_true_bool(true)).should_be_true();
	}*/
});