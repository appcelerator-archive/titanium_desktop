describe("Ruby Tests",
{
	test_inline: function()
	{
		value_of(window.inline_test_result).should_be('A');
	},
	test_external_file: function()
	{
		value_of(window.external_test_result).should_be('A');
	},
	test_require_file_module: function()
	{
		value_of(window.require_file_module_result).should_be('hello world');
	},
	test_require_file_module_and_sub_file: function()
	{
		value_of(window.require_file_sub_file_module_result).should_be('yah');
	},
	test_window_global_from_ruby: function()
	{
		// test to make sure that we can access a function defined
		// in normal javascript block from within ruby 
		value_of(window.test_window_global_result).should_be('you suck ass');
	},
	test_window_global_var_from_ruby: function()
	{
		// test passing global variable from JS and getting it back
		value_of(window.what_is_love_result).should_be('i love you');
	},
	test_document_title_from_ruby: function()
	{
		value_of(window.test_document_title_result).should_be('I love Matz');
	},
	test_gem_include: function()
	{
		value_of(window.test_gem_result).should_be('<html><head><meta content="text/html; charset=utf-8" http-equiv="Content-Type"/><title>Boats.com</title></head></html>');
	},
	test_type_string: function()
	{
		value_of(test_type_string()).should_be_string();
		value_of(test_type_string()).should_be('i am string');
	},
	test_type_int: function()
	{
		value_of(test_type_int()).should_be_number();
		value_of(test_type_int()).should_be_exactly(1);
	},
	test_type_float: function()
	{
		value_of(test_type_float()).should_be_number();
		value_of(test_type_float()).should_be_exactly(1.1);
	},
	test_type_long: function()
	{
		value_of(test_type_long()).should_be_number();
		// google
		value_of(test_type_long()).should_be_exactly(10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000);
	},
	test_type_scientific_notation_large: function()
	{
		value_of(test_type_scientific_notation_large()).should_be_number();
		value_of(test_type_scientific_notation_large()).should_be_exactly(17000000000000000000);
	},
	test_type_scientific_notation_small: function()
	{
		value_of(test_type_scientific_notation_small()).should_be_number();
		value_of(test_type_scientific_notation_small()).should_be_exactly(3.21e-13);
	},
	test_type_boolean_false: function()
	{
		value_of(test_type_boolean_false()).should_be_boolean();
		value_of(test_type_boolean_false()).should_be_false();
	},
	test_type_boolean_true: function()
	{
		value_of(test_type_boolean_true()).should_be_boolean();
		value_of(test_type_boolean_true()).should_be_true();
	},
	test_type_map: function()
	{
		value_of(test_type_map()).should_be_object();
		value_of(test_type_map().a).should_be('b');
	},
	test_type_tuple: function()
	{
		var tuple = test_type_tuple();
		value_of(tuple).should_be_object();
		value_of(tuple.one()).should_be(2);
	},
	test_type_struct: function()
	{
		var struct = test_type_struct();
		value_of(struct).should_be_object();
		value_of(struct.name()).should_be("jeff");
	},
	test_type_nil: function()
	{
		value_of(test_type_nil()).should_be_null();
	},
	test_type_array: function()
	{
		value_of(test_type_array()).should_be_object();
		value_of(test_type_array().length).should_be(3);
		value_of(test_type_array()[0]).should_be_exactly(1);
		value_of(test_type_array()[1]).should_be_exactly(2);
		value_of(test_type_array()[2]).should_be_exactly(3);
	},
	test_type_function: function()
	{
		value_of(test_type_function()).should_be_function();
	},
	test_type_proc: function()
	{
		var p = test_type_proc();
		value_of(p).should_be_object();
		// try invoking the returned proc
		value_of(p.call()).should_be('hello world');
	},
	test_js_type_string: function()
	{
		value_of(test_js_type_string('1')).should_be_true();
	},
	test_js_type_int: function()
	{
		value_of(test_js_type_float(123)).should_be_true();
	},
	test_js_type_zero: function()
	{
		value_of(test_js_type_float(0)).should_be_true();
	},
	test_js_type_large_number: function()
	{
		value_of(test_js_type_float(10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)).should_be_true();
	},
	test_js_type_float: function()
	{
		value_of(test_js_type_float(3.14)).should_be_true();
	},
	test_js_type_function: function()
	{
		value_of(test_js_type_kmethod(function() { } )).should_be_true();
	},
	test_js_type_klist: function()
	{
		value_of(test_js_type_klist([1,2,3])).should_be_true();
	},
	test_js_type_dict: function()
	{
		value_of(test_js_type_kobject({'a1':'sauce'})).should_be_true();
	},
	test_js_type_object: function()
	{
		value_of(test_js_type_kobject({})).should_be_true();
	},
	test_js_type_null: function()
	{
		value_of(test_js_type_nil(null)).should_be_true();
	},
	test_js_type_undefined: function()
	{
		value_of(test_js_type_nil(undefined)).should_be_true();
	},
	test_js_type_true: function()
	{
		value_of(test_js_type_true_bool(true)).should_be_true();
	},
	test_js_type_false: function()
	{
		value_of(test_js_type_false_bool(false)).should_be_true();
	},
});
