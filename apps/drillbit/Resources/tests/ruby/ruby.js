describe("Ruby Tests",
{
	test_resources_directory_in_include_path: function()
	{
		var include_path = get_include_path();
		var res_dir = Titanium.API.getApplication().getResourcesPath();
		value_of(include_path.indexOf(res_dir) != -1).should_be_true();
	},
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
		value_of(p).should_be_function();
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
	test_js_klist_elements: function()
	{
		value_of(test_js_klist_elements([1,2,3])).should_be_true();
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
	test_rubykobject_respond_to: function()
	{
		a = Object();
		a.sheep = "baaah";
		a.cow = function() { return 'moo'; };
		a.phillip = 34;
		a.undef = undefined;
		value_of(test_rubykobject_respond_to(a)).should_be("");
	},
	test_rubykobject_method_missing_exception: function()
	{
		a = Object();
		a.sheep = "baaah";
		a.cow = function() { return 'moo'; };
		a.phillip = 34;
		a.undef = undefined;
		value_of(test_rubykobject_method_missing_exception(a)).should_be("");
	},
	test_rubyklist_length: function()
	{
		a = [];
		value_of(test_rubyklist_length(a, 0)).should_be("");

		a = [0, 1, 2, 3];
		value_of(test_rubyklist_length(a, 4)).should_be("");

		a[99] = "blah";
		value_of(test_rubyklist_length(a, 100)).should_be("");
	},
	test_rubyklist_each: function()
	{
		a = [];
		value_of(test_rubyklist_each(a)).should_be("");

		a = [0, 1, 2, 3];
		value_of(test_rubyklist_each(a, 0, 1, 2, 3)).should_be("");

		a[9] = "blah";
		value_of(test_rubyklist_each(a, 0, 1, 2, 3, undefined, undefined, undefined, undefined, undefined, "blah")).should_be("");
	},
	test_ruby_proc: function()
	{
		var procMethod = get_ruby_proc();
		var x = procMethod("duh");
		value_of(x).should_be("duhfoo!");
	},
	test_ruby_arity: function()
	{
		var procMethod = get_ruby_proc_arity(0);
		value_of(procMethod()).should_be("|");

		procMethod = get_ruby_proc_arity(1);
		value_of(procMethod("boo")).should_be("boo|");
		procMethod = get_ruby_proc_arity(2);
		value_of(procMethod("boo", "foo")).should_be("boo|foo|");
		procMethod = get_ruby_proc_arity(3);
		value_of(procMethod("boo", "foo", "doo")).should_be("boo|foo|doo|");
		procMethod = get_ruby_proc_arity(-1);
		value_of(procMethod("boo", "foo", "doo")).should_be("boo|foo|doo|");
		procMethod = get_ruby_proc_arity(-2);
		value_of(procMethod("boo", "foo", "doo")).should_be("boo|foo|doo|");
	},
	test_krubyobject_iterate_properties: function()
	{
		var count_properties = function(o) { var n = 0; for (var x in o) { n++; } return n; };
		var obj = get_empty_ruby_object();

		// Iterating through Ruby properties many times sometimes casues a segfault
		var orig_property_count = count_properties(obj);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
		value_of(count_properties(obj)).should_be(orig_property_count);
	},
	test_empty_krubyobject: function()
	{
		var count_properties = function(o) { var n = 0; for (var x in o) { n++; } return n; };
		var o = get_empty_ruby_object();
		// There should be no properties showing for a blank KObject
		var orig_property_count = count_properties(o);

		// Yet there are some hidden methods that should be there
		value_of(o.equals).should_be_function();
		value_of(o.toString).should_be_function();

		value_of(o.equals(o)).should_be_true();

		var other = Object();
		value_of(o.equals(other)).should_be_false();
		other = get_empty_ruby_object();
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

		// Ruby does not support setting variable names which have whitespace
		//o['		 weird property name	   '] = "oh noes";
		//value_of(o['		 weird property name	   ']).should_be("oh noes");
	},
	test_empty_krubyhash: function()
	{
		var count_properties = function(o) { var n = 0; for (var x in o) { n++; } return n; };
		var o = get_empty_ruby_hash();
		value_of(true).should_be(true);
		// There should be no properties showing for a blank KObject
		var orig_property_count = count_properties(o);

		// Yet there are some hidden methods that should be there
		value_of(o.equals).should_be_function();
		value_of(o.toString).should_be_function();

		value_of(o.equals(o)).should_be_true();

		var other = Object();
		value_of(o.equals(other)).should_be_false();
		other = get_empty_ruby_hash();
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
	},
	test_empty_krubylist: function()
	{
		var l = get_empty_ruby_list();
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
		var l = get_empty_ruby_list();
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
