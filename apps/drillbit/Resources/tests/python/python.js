describe("Python Tests",
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
		value_of(window.require_file_module_result).should_be('hello,world');
	},
	test_require_file_module_and_sub_file: function()
	{
		value_of(window.require_file_sub_file_module_result).should_be('yah');
	},
	test_window_global_from_python: function()
	{
		// test to make sure that we can access a function defined
		// in normal javascript block from within python 
		value_of(window.test_window_global_result).should_be('you suck ass');
	},
	test_window_global_var_from_python: function()
	{
		// test passing global variable from JS and getting it back
		value_of(window.what_is_love_result).should_be('i love you');
	},
	test_document_title_from_python: function()
	{
		value_of(window.test_document_title_result).should_be('I love Guido');
	},
	test_module_include: function()
	{
		value_of(window.test_external_module_result).should_be_object();
		value_of(window.test_external_module_result.length).should_be(3);
		value_of(window.test_external_module_result[0]).should_be('android');
		value_of(window.test_external_module_result[1]).should_be('gphone');
		value_of(window.test_external_module_result[2]).should_be('cupcake');
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
	test_type_long: function()
	{
		value_of(test_type_long()).should_be_number();
		// the google
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
		value_of(test_type_tuple()).should_be_object();
		value_of(test_type_tuple().length).should_be(2);
		value_of(test_type_tuple()[0]).should_be('a');
		value_of(test_type_tuple()[1]).should_be('b');
	},
	test_type_none: function()
	{
		value_of(test_type_none()).should_be_null();
	},
	test_type_dict: function()
	{
		value_of(test_type_dict()).should_be_object();
		value_of(test_type_dict()['one']).should_be_exactly(2);
		value_of(test_type_dict().one).should_be_exactly(2);
		value_of(test_type_dict()['two']).should_be_exactly(3);
		value_of(test_type_dict().two).should_be_exactly(3);
	},
	test_type_function: function()
	{
		value_of(test_type_function()).should_be_function();
		value_of(test_type_function()()).should_be_object();
	},
	test_type_anonymous_function: function()
	{
		value_of(test_type_anonymous_function()).should_be_function();
		value_of(test_type_anonymous_function()()).should_be('foobar');
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
	test_js_type_list: function()
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
		value_of(test_js_type_none(null)).should_be_true();
	},
	test_js_type_undefined: function()
	{
		value_of(test_js_type_none(undefined)).should_be_true();
	},
	test_js_type_true: function()
	{
		value_of(test_js_type_bool(true)).should_be_true();
	},
	test_js_type_false: function()
	{
		value_of(test_js_type_bool(false)).should_be_true();
	},
	test_py_override_delegators: function()
	{
		value_of(Titanium.UI.currentWindow).should_be_object();
	},
	test_python_list: function()
	{
		var list = get_python_list();
		value_of(list[0]).should_be(1);
		value_of(list[1]).should_be(2);
		value_of(list[2]).should_be(3);
		value_of(list[3]).should_be_undefined();
		value_of(list[300]).should_be_undefined();
		
	},
	test_js_array_sum: function()
	{
		var x = new Array();
		x[0] = 1;
		x[1] = 3;
		x[2] = 4;
		var sum = js_array_sum(x);
		value_of(sum).should_be(8);
	},
	test_js_array_sum2: function()
	{
		var x = [1, 2, 3]
		var sum = js_array_sum(x);
		value_of(sum).should_be(6);
	},
	test_empty_kpythonobject: function()
	{
		var count_properties = function(o) { var n = 0; for (x in o) { n++; } return n; };
		var o = get_empty_python_object();
		// There should be no properties showing for a blank KObject
		var orig_property_count = count_properties(o);

		// Yet there are some hidden methods that should be there
		value_of(o.equals).should_be_function();
		value_of(o.toString).should_be_function();

		value_of(o.equals(o)).should_be_true();

		var other = Object();
		value_of(o.equals(other)).should_be_false();
		other = get_empty_python_object();
		value_of(o.equals(other)).should_be_false();
		value_of(o.toString()).should_be_string();

		o.property_one = "Foo!";
		value_of(o.property_one).should_be("Foo!");
		value_of(count_properties(o)).should_be(orig_property_count + 1);
		o.property_one = 123;
		value_of(o.property_one).should_be(123);
		value_of(count_properties(o)).should_be(orig_property_count + 1);
		o.property_one = o;
		value_of(o.equals(o.property_one)).should_be_true();
		value_of(count_properties(o)).should_be(orig_property_count + 1);

		o.property_two = "Foo2!";
		value_of(o.property_two).should_be("Foo2!");
		value_of(count_properties(o)).should_be(orig_property_count + 2);

		o.property_one = "blahblah";
		value_of(o.property_one).should_be("blahblah");

		o['		 weird property name	   '] = "oh noes";
		value_of(o['		 weird property name	   ']).should_be("oh noes");
	},
	test_empty_kpythondict: function()
	{
		var count_properties = function(o) { var n = 0; for (x in o) { n++; } return n; };
		var o = get_empty_python_dict();
		// There should be no properties showing for a blank KObject
		var orig_property_count = count_properties(o);

		// Yet there are some hidden methods that should be there
		value_of(o.equals).should_be_function();
		value_of(o.toString).should_be_function();

		value_of(o.equals(o)).should_be_true();

		var other = Object();
		value_of(o.equals(other)).should_be_false();
		other = get_empty_python_dict();
		value_of(o.equals(other)).should_be_false();
		value_of(o.toString()).should_be_string();

		o.property_one = "Foo!";
		value_of(o.property_one).should_be("Foo!");
		value_of(count_properties(o)).should_be(orig_property_count + 1);
		o.property_one = 123;
		value_of(o.property_one).should_be(123);
		value_of(count_properties(o)).should_be(orig_property_count + 1);
		o.property_one = o;
		value_of(o.equals(o.property_one)).should_be_true();
		value_of(count_properties(o)).should_be(orig_property_count + 1);

		o.property_two = "Foo2!";
		value_of(o.property_two).should_be("Foo2!");
		value_of(count_properties(o)).should_be(orig_property_count + 2);

		o.property_one = "blahblah";
		value_of(o.property_one).should_be("blahblah");

		o['		 weird property name	   '] = "oh noes";
		value_of(o['		 weird property name	   ']).should_be("oh noes");
	},
	test_empty_kpythonlist: function()
	{
		var l = get_empty_python_list();
		value_of(l.length).should_be_number();
		value_of(l.equals).should_be_function();
		value_of(l.toString).should_be_function();
		value_of(l.length).should_be(0);

		value_of(l.pop).should_be_function();
		value_of(l.push).should_be_function();
		value_of(l.reverse).should_be_function();
		value_of(l.shift).should_be_function();
		value_of(l.sort).should_be_function();
		value_of(l.splice).should_be_function();
		value_of(l.unshift).should_be_function();
		value_of(l.concat).should_be_function();
		value_of(l.join).should_be_function();
		value_of(l.slice).should_be_function();
	},
	test_modifying_klist: function()
	{
		var l = get_empty_python_list();
		value_of(l.length).should_be_number();
		value_of(l.length).should_be(0);
		l.push(123);
		value_of(l.length).should_be(1);
		value_of(l[0]).should_be(123);

		l.push("abc");
		value_of(l.length).should_be(2);
		value_of(l[1]).should_be("abc");

		var popped = l.pop();
		value_of(popped).should_be("abc");
		value_of(l.length).should_be(1);
		value_of(l[0]).should_be(123);

		l.length = 3;
		value_of(l.length).should_be(3);
		value_of(l[0]).should_be(123);
		value_of(l[1]).should_be(undefined);
		value_of(l[2]).should_be(undefined);

		l[2] = "blah";
		value_of(l[0]).should_be(123);
		value_of(l[1]).should_be(undefined);
		value_of(l[2]).should_be("blah");

		l.length = 1;

		l.push(l);
		value_of(l.length).should_be(2);
		value_of(l[1].equals(l)).should_be_true();

		popped = l.pop();
		value_of(popped.equals(l)).should_be_true();

		l[20] = "blah";
		value_of(l.length).should_be(21);

		l.length = 0;
		value_of(l.length).should_be(0);
	}
});
