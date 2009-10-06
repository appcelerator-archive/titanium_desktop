describe("Blob, KObject, KList, etc",{
	test_core_types_harness: function()
	{
		value_of(Titanium.API.createKObject).should_be_function();
		value_of(Titanium.API.createKMethod).should_be_function();
		value_of(Titanium.API.createKList).should_be_function();
		value_of(Titanium.API.createBlob).should_be_function();
	},
	test_empty_kobject: function()
	{
		var count_properties = function(o) { var n = 0; for (x in o) { n++; } return n; };
		var o = Titanium.API.createKObject();
		// There should be no properties showing for a blank KObject
		value_of(count_properties(o)).should_be(0);

		// Yet there are some hidden methods that should be there
		value_of(o.equals).should_be_function();
		value_of(o.toString).should_be_function();

		value_of(o.equals(o)).should_be_true();

		var other = Object();
		value_of(o.equals(other)).should_be_false();
		other = Titanium.API.createKObject();
		value_of(o.equals(other)).should_be_false();
		value_of(o.toString()).should_be_string();

		o.property_one = "Foo!";
		value_of(o.property_one).should_be("Foo!");
		value_of(count_properties(o)).should_be(1);
		o.property_one = 123;
		value_of(o.property_one).should_be(123);
		value_of(count_properties(o)).should_be(1);
		o.property_one = o;
		value_of(o.equals(o.property_one)).should_be_true();
		value_of(count_properties(o)).should_be(1);

		o.property_two = "Foo2!";
		value_of(o.property_two).should_be("Foo2!");
		value_of(count_properties(o)).should_be(2);
	},
	test_wrapped_kobject: function()
	{
		var count_properties = function(o) { var n = 0; for (x in o) { n++; } return n; };
		var o = Object();
		o.property_one = "blahblah";
		var ko = Titanium.API.createKObject(o);

		value_of(count_properties(ko)).should_be(1);
		value_of(count_properties(o)).should_be(1);
		value_of(ko.property_one).should_be("blahblah");
		value_of(o.property_one).should_be("blahblah");
		value_of(ko['property_one']).should_be("blahblah");
		value_of(o['property_one']).should_be("blahblah");

		o['		 weird property name	   '] = "oh noes";
		value_of(o['		 weird property name	   ']).should_be("oh noes");
		value_of(ko['		 weird property name	   ']).should_be("oh noes");
	},
	test_empty_klist: function()
	{
		var l = Titanium.API.createKList();
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
		var l = Titanium.API.createKList();
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
	},
	test_wrapped_klist: function()
	{
		var mylist = [1, 2, 3];
		var l = Titanium.API.createKList(mylist);
		value_of(l.length).should_be_number();
		value_of(l.equals).should_be_function();
		value_of(l.toString).should_be_function();
		value_of(l.length).should_be(3);

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

		value_of(l.length).should_be(3);
		value_of(mylist.length).should_be(3);

		var popped = l.pop();
		value_of(popped).should_be(3);
		value_of(l.length).should_be(2);
		value_of(mylist.length).should_be(2);
		var popped = l.pop();
		value_of(popped).should_be(2);
		value_of(l.length).should_be(1);
		value_of(mylist.length).should_be(1);
		var popped = l.pop();
		value_of(popped).should_be(1);
		value_of(l.length).should_be(0);
		value_of(mylist.length).should_be(0);

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
	},
	test_kmethod_closure: function()
	{
		var variable = "uno";
		var other_variable = "yes";
		var myfunction = function()
		{
			other_variable = "no";
			return variable;
		}

		variable = "dos";
		var f = Titanium.API.createKMethod(myfunction);
		var result = f();
		value_of(result).should_be("dos");
		value_of(other_variable).should_be("no");
	},
	test_kmethod_closure_superduper_as_async: function(callback)
	{
		var variable = "uno";
		var other_variable = "yes";
		var myfunction = function()
		{
			other_variable = "no";
			return variable;
		};
		var myfunction2 = function()
		{
			return myfunction;
		};

		setTimeout(function()
		{
			variable = "dos";
			var f = Titanium.API.createKMethod(myfunction2());
			var result = f();

			if (result !== "dos")
			{
				callback.failed("Closure FAIL");
			}
			if (other_variable !== "no")
			{
				callback.failed("Closure FAIL2");
			}
			callback.passed();
		}, 50);
	},
	test_basic_empty_blob: function()
	{
		var b1 = Titanium.API.createBlob();
		value_of(b1).should_be_object();
		value_of(b1.length).should_be_number();
		value_of(b1.toString).should_be_function();
		value_of(b1.get).should_be_function();
		value_of(b1.indexOf).should_be_function();
		value_of(b1.lastIndexOf).should_be_function();
		value_of(b1.charAt).should_be_function();
		value_of(b1.split).should_be_function();
		value_of(b1.substring).should_be_function();
		value_of(b1.substr).should_be_function();
		value_of(b1.toLowerCase).should_be_function();
		value_of(b1.toUpperCase).should_be_function();
		value_of(b1.length).should_be(0);
		value_of(b1.toString()).should_be_string();
		value_of(b1.toString()).should_be("");
	},
	test_basic_blob: function()
	{
		var b1 = Titanium.API.createBlob("abcdefgA");
		value_of(b1).should_be_object();
		value_of(b1.length).should_be_number();
		value_of(b1.toString).should_be_function();
		value_of(b1.get).should_be_function();
		value_of(b1.indexOf).should_be_function();
		value_of(b1.lastIndexOf).should_be_function();
		value_of(b1.charAt).should_be_function();
		value_of(b1.split).should_be_function();
		value_of(b1.substring).should_be_function();
		value_of(b1.substr).should_be_function();
		value_of(b1.toLowerCase).should_be_function();
		value_of(b1.toUpperCase).should_be_function();
		value_of(b1.length).should_be(8);
		value_of(b1.toString()).should_be_string();
		value_of(b1.toString()).should_be("abcdefgA");
	},
	test_blob_indexof: function()
	{
		// must conform to behavior:
		// https://developer.mozilla.org/en/core_javascript_1.5_reference/global_objects/string/chara://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Global_Objects/String/indexOf
		var b1 = Titanium.API.createBlob("abcdefgA");
		value_of(b1.indexOf).should_be_function();
		value_of(b1.indexOf("a")).should_be(0);
		value_of(b1.indexOf("b")).should_be(1);
		value_of(b1.indexOf("c")).should_be(2);
		value_of(b1.indexOf("g")).should_be(6);
		value_of(b1.indexOf("A")).should_be(7);
		value_of(b1.indexOf("Z")).should_be(-1);
		value_of(b1.indexOf("*")).should_be(-1);
		value_of(b1.indexOf("B")).should_be(-1);
		value_of(b1.indexOf("abc")).should_be(0);
		value_of(b1.indexOf("bcd")).should_be(1);
		value_of(b1.indexOf("cd")).should_be(2);
		value_of(b1.indexOf("def")).should_be(3);
		value_of(b1.indexOf("defe")).should_be(-1);
		value_of(b1.indexOf("fgA")).should_be(5);
		value_of(b1.indexOf("a", -100)).should_be(0);
		value_of(b1.indexOf("b", -100)).should_be(1);
		value_of(b1.indexOf("c", 0)).should_be(2);
		value_of(b1.indexOf("c", 2)).should_be(2);
		value_of(b1.indexOf("c", 3)).should_be(-1);
		value_of(b1.indexOf("g", 2)).should_be(6);
		value_of(b1.indexOf("g", 7)).should_be(-1);
		value_of(b1.indexOf("A", 8)).should_be(-1);
		value_of(b1.indexOf("Z", 8)).should_be(-1);
		value_of(b1.indexOf("abc", 0)).should_be(0);
		value_of(b1.indexOf("abc", 1)).should_be(-1);
		value_of(b1.indexOf("bcd", 0)).should_be(1);
		value_of(b1.indexOf("bcd", 30)).should_be(-1);
		value_of(b1.indexOf("defe", 1)).should_be(-1);

		var b2 = Titanium.API.createBlob("");
		value_of(b2.indexOf).should_be_function();
		value_of(b2.indexOf("a")).should_be(-1);
		value_of(b2.indexOf("b")).should_be(-1);
		value_of(b2.indexOf("c")).should_be(-1);
		value_of(b2.indexOf("g")).should_be(-1);
		value_of(b2.indexOf("A")).should_be(-1);
		value_of(b2.indexOf("Z")).should_be(-1);
		value_of(b2.indexOf("*")).should_be(-1);
		value_of(b2.indexOf("B")).should_be(-1);
		value_of(b2.indexOf("abc")).should_be(-1);
		value_of(b2.indexOf("bcd")).should_be(-1);
		value_of(b2.indexOf("cd")).should_be(-1);
		value_of(b2.indexOf("def")).should_be(-1);
		value_of(b2.indexOf("defe")).should_be(-1);
		value_of(b2.indexOf("fgA")).should_be(-1);
		value_of(b2.indexOf("a", -100)).should_be(-1);
		value_of(b2.indexOf("b", -100)).should_be(-1);
		value_of(b2.indexOf("c", 0)).should_be(-1);
		value_of(b2.indexOf("c", 2)).should_be(-1);
		value_of(b2.indexOf("c", 3)).should_be(-1);
		value_of(b2.indexOf("g", 2)).should_be(-1);
		value_of(b2.indexOf("g", 7)).should_be(-1);
		value_of(b2.indexOf("A", 8)).should_be(-1);
		value_of(b2.indexOf("Z", 8)).should_be(-1);
		value_of(b2.indexOf("abc", 0)).should_be(-1);
		value_of(b2.indexOf("abc", 1)).should_be(-1);
		value_of(b2.indexOf("bcd", 0)).should_be(-1);
		value_of(b2.indexOf("bcd", 30)).should_be(-1);
		value_of(b2.indexOf("defe", 1)).should_be(-1);
	},
	test_blob_lastindexof: function()
	{
		// must conform to behavior:
		// https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Global_Objects/String/lastIndexOf
		var b1 = Titanium.API.createBlob("abcdefgA");
		value_of(b1.lastIndexOf).should_be_function();
		value_of(b1.lastIndexOf("a")).should_be(0);
		value_of(b1.lastIndexOf("b")).should_be(1);
		value_of(b1.lastIndexOf("c")).should_be(2);
		value_of(b1.lastIndexOf("g")).should_be(6);
		value_of(b1.lastIndexOf("A")).should_be(7);
		value_of(b1.lastIndexOf("Z")).should_be(-1);
		value_of(b1.lastIndexOf("*")).should_be(-1);
		value_of(b1.lastIndexOf("B")).should_be(-1);
		value_of(b1.lastIndexOf("abc")).should_be(0);
		value_of(b1.lastIndexOf("bcd")).should_be(1);
		value_of(b1.lastIndexOf("cd")).should_be(2);
		value_of(b1.lastIndexOf("def")).should_be(3);
		value_of(b1.lastIndexOf("defe")).should_be(-1);
		value_of(b1.lastIndexOf("fgA")).should_be(5);
		value_of(b1.lastIndexOf("a", -100)).should_be(0);
		value_of(b1.lastIndexOf("b", -100)).should_be(-1);
		value_of(b1.lastIndexOf("a", 100)).should_be(0);
		value_of(b1.lastIndexOf("b", 100)).should_be(1);
		value_of(b1.lastIndexOf("c", 0)).should_be(-1);
		value_of(b1.lastIndexOf("c", 2)).should_be(2);
		value_of(b1.lastIndexOf("c", 3)).should_be(2);
		value_of(b1.lastIndexOf("g", 1)).should_be(-1);
		value_of(b1.lastIndexOf("g", 2)).should_be(-1);
		value_of(b1.lastIndexOf("g", 7)).should_be(6);
		value_of(b1.lastIndexOf("A", 8)).should_be(7);
		value_of(b1.lastIndexOf("Z", 8)).should_be(-1);
		value_of(b1.lastIndexOf("abc", 0)).should_be(0);
		value_of(b1.lastIndexOf("abc", 1)).should_be(0);
		value_of(b1.lastIndexOf("bcd", 0)).should_be(-1);
		value_of(b1.lastIndexOf("bcd", 30)).should_be(1);
		value_of(b1.lastIndexOf("defe", 1)).should_be(-1);

		var b2 = Titanium.API.createBlob("");
		value_of(b2.lastIndexOf).should_be_function();
		value_of(b2.lastIndexOf("a")).should_be(-1);
		value_of(b2.lastIndexOf("b")).should_be(-1);
		value_of(b2.lastIndexOf("c")).should_be(-1);
		value_of(b2.lastIndexOf("g")).should_be(-1);
		value_of(b2.lastIndexOf("A")).should_be(-1);
		value_of(b2.lastIndexOf("Z")).should_be(-1);
		value_of(b2.lastIndexOf("*")).should_be(-1);
		value_of(b2.lastIndexOf("B")).should_be(-1);
		value_of(b2.lastIndexOf("abc")).should_be(-1);
		value_of(b2.lastIndexOf("bcd")).should_be(-1);
		value_of(b2.lastIndexOf("cd")).should_be(-1);
		value_of(b2.lastIndexOf("def")).should_be(-1);
		value_of(b2.lastIndexOf("defe")).should_be(-1);
		value_of(b2.lastIndexOf("fgA")).should_be(-1);
		value_of(b2.lastIndexOf("a", -100)).should_be(-1);
		value_of(b2.lastIndexOf("b", -100)).should_be(-1);
		value_of(b2.lastIndexOf("c", 0)).should_be(-1);
		value_of(b2.lastIndexOf("c", 2)).should_be(-1);
		value_of(b2.lastIndexOf("c", 3)).should_be(-1);
		value_of(b2.lastIndexOf("g", 2)).should_be(-1);
		value_of(b2.lastIndexOf("g", 7)).should_be(-1);
		value_of(b2.lastIndexOf("A", 8)).should_be(-1);
		value_of(b2.lastIndexOf("Z", 8)).should_be(-1);
		value_of(b2.lastIndexOf("abc", 0)).should_be(-1);
		value_of(b2.lastIndexOf("abc", 1)).should_be(-1);
		value_of(b2.lastIndexOf("bcd", 0)).should_be(-1);
		value_of(b2.lastIndexOf("bcd", 30)).should_be(-1);
		value_of(b2.lastIndexOf("defe", 1)).should_be(-1);

		var b3 = Titanium.API.createBlob("abcdefgAadef");
		value_of(b3.lastIndexOf).should_be_function();
		value_of(b3.lastIndexOf("a")).should_be(8);
		value_of(b3.lastIndexOf("b")).should_be(1);
		value_of(b3.lastIndexOf("c")).should_be(2);
		value_of(b3.lastIndexOf("g")).should_be(6);
		value_of(b3.lastIndexOf("A")).should_be(7);
		value_of(b3.lastIndexOf("Z")).should_be(-1);
		value_of(b3.lastIndexOf("*")).should_be(-1);
		value_of(b3.lastIndexOf("B")).should_be(-1);
		value_of(b3.lastIndexOf("abc")).should_be(0);
		value_of(b3.lastIndexOf("bcd")).should_be(1);
		value_of(b3.lastIndexOf("cd")).should_be(2);
		value_of(b3.lastIndexOf("def")).should_be(9);
		value_of(b3.lastIndexOf("def", 30)).should_be(9);
		value_of(b3.lastIndexOf("defe")).should_be(-1);
		value_of(b3.lastIndexOf("fgA")).should_be(5);
		value_of(b3.lastIndexOf("a", 100)).should_be(8);
		value_of(b3.lastIndexOf("b", 100)).should_be(1);
		value_of(b3.lastIndexOf("a", -100)).should_be(0);
		value_of(b3.lastIndexOf("b", -100)).should_be(-1);
		value_of(b3.lastIndexOf("c", 0)).should_be(-1);
		value_of(b3.lastIndexOf("c", 2)).should_be(2);
		value_of(b3.lastIndexOf("c", 3)).should_be(2);
		value_of(b3.lastIndexOf("g", 2)).should_be(-1);
		value_of(b3.lastIndexOf("g", 7)).should_be(6);
		value_of(b3.lastIndexOf("A", 8)).should_be(7);
		value_of(b3.lastIndexOf("Z", 8)).should_be(-1);
		value_of(b3.lastIndexOf("abc", 0)).should_be(0);
		value_of(b3.lastIndexOf("abc", 1)).should_be(0);
		value_of(b3.lastIndexOf("bcd", 0)).should_be(-1);
		value_of(b3.lastIndexOf("bcd", 30)).should_be(1);
		value_of(b3.lastIndexOf("defe", 1)).should_be(-1);
	},
	test_blob_charat: function()
	{
		var b1 = Titanium.API.createBlob("abcdefg");
		value_of(b1.charAt(-100)).should_be("");
		value_of(b1.charAt(-1)).should_be("");
		value_of(b1.charAt(0)).should_be("a");
		value_of(b1.charAt(1)).should_be("b");
		value_of(b1.charAt(2)).should_be("c");
		value_of(b1.charAt(3)).should_be("d");
		value_of(b1.charAt(4)).should_be("e");
		value_of(b1.charAt(5)).should_be("f");
		value_of(b1.charAt(6)).should_be("g");
		value_of(b1.charAt(7)).should_be("");
		value_of(b1.charAt(700)).should_be("");

		var b2 = Titanium.API.createBlob("");
		value_of(b2.charAt(-100)).should_be("");
		value_of(b2.charAt(-1)).should_be("");
		value_of(b2.charAt(0)).should_be("");
		value_of(b2.charAt(1)).should_be("");
		value_of(b2.charAt(2)).should_be("");
		value_of(b2.charAt(3)).should_be("");
		value_of(b2.charAt(4)).should_be("");
		value_of(b2.charAt(5)).should_be("");
		value_of(b2.charAt(6)).should_be("");
		value_of(b2.charAt(7)).should_be("");
		value_of(b2.charAt(700)).should_be("");
	},
	test_blob_split: function()
	{
		var b1 = Titanium.API.createBlob("abcdefg");
		var b1s = b1.split();
		value_of(b1s).should_be_object();
		value_of(b1s.length).should_be(1);
		value_of(b1s[0]).should_be("abcdefg");

		var b2 = Titanium.API.createBlob("");
		var b2s = b2.split();
		value_of(b2s.length).should_be(1);
		value_of(b2s[0]).should_be("");

		var b3 = Titanium.API.createBlob("abcdefg");
		var b3s = b3.split(",");
		value_of(b3s).should_be_object();
		value_of(b3s.length).should_be(1);
		value_of(b3s[0]).should_be("abcdefg");

		var b4 = Titanium.API.createBlob("ab,cdefg");
		var b4s = b4.split(",");
		value_of(b4s).should_be_object();
		value_of(b4s.length).should_be(2);
		value_of(b4s[0]).should_be("ab");
		value_of(b4s[1]).should_be("cdefg");

		var b5 = Titanium.API.createBlob(",ab,cdefg,,");
		var b5s = b5.split(",");
		value_of(b5s).should_be_object();
		value_of(b5s.length).should_be(5);
		value_of(b5s[0]).should_be("");
		value_of(b5s[1]).should_be("ab");
		value_of(b5s[2]).should_be("cdefg");
		value_of(b5s[3]).should_be("");
		value_of(b5s[4]).should_be("");

		var b6 = Titanium.API.createBlob(",ab,cdefg,");
		var b6s = b6.split(",", 2);
		value_of(b6s).should_be_object();
		value_of(b6s.length).should_be(2);
		value_of(b6s[0]).should_be("");
		value_of(b6s[1]).should_be("ab");

		var b7 = Titanium.API.createBlob("abc,def,ghi");
		var b7s = b7.split(",", 0);
		value_of(b7s).should_be_object();
		value_of(b7s.length).should_be(0);

		var b8 = Titanium.API.createBlob("abcde");
		var b8s = b8.split("");
		value_of(b8s).should_be_object();
		value_of(b8s.length).should_be(5);
		value_of(b8s[0]).should_be('a');
		value_of(b8s[1]).should_be('b');
		value_of(b8s[2]).should_be('c');
		value_of(b8s[3]).should_be('d');
		value_of(b8s[4]).should_be('e');
	},
	test_blob_substr: function()
	{
		var blob = Titanium.API.createBlob("abcdefghij");
		value_of(blob.substr).should_be_function();
		value_of(blob.substr(1,2)).should_be("bc");
		value_of(blob.substr(-3,2)).should_be("hi");
		value_of(blob.substr(-3)).should_be("hij");
		value_of(blob.substr(1)).should_be("bcdefghij");
		value_of(blob.substr(-20,2)).should_be("ab");
		value_of(blob.substr(20,2)).should_be("");
		value_of(blob.substr(1,-2)).should_be("");
		value_of(blob.substr(1,0)).should_be("");
	},
	test_blob_substring: function()
	{
		var blob = Titanium.API.createBlob("Mozilla");
		value_of(blob.substring).should_be_function();
		value_of(blob.substring(3,0)).should_be("Moz");
		value_of(blob.substring(0,3)).should_be("Moz");
		value_of(blob.substring(-10,3)).should_be("Moz");
		value_of(blob.substring(4,7)).should_be("lla");
		value_of(blob.substring(7,4)).should_be("lla");
		value_of(blob.substring(0,6)).should_be("Mozill");
		value_of(blob.substring(6,0)).should_be("Mozill");
		value_of(blob.substring(0,7)).should_be("Mozilla");
		value_of(blob.substring(0,10)).should_be("Mozilla");
		value_of(blob.substring(7,0)).should_be("Mozilla");
		value_of(blob.substring(10,0)).should_be("Mozilla");
	},
	test_blob_tolowercase: function()
	{
		var blob = Titanium.API.createBlob("Mozilla123!?");
		value_of(blob.toLowerCase).should_be_function();
		value_of(blob.toLowerCase()).should_be("mozilla123!?");
		blob = Titanium.API.createBlob("mOZILLA123!?");
		value_of(blob.toLowerCase()).should_be("mozilla123!?");
		blob = Titanium.API.createBlob("mO   ZILLA123!?");
		value_of(blob.toLowerCase()).should_be("mo   zilla123!?");
		blob = Titanium.API.createBlob("1234567890-=!@#$%^&*()_+");
		value_of(blob.toLowerCase()).should_be("1234567890-=!@#$%^&*()_+");
	},
	test_blob_touppercase: function()
	{
		var blob = Titanium.API.createBlob("Mozilla123!?");
		value_of(blob.toUpperCase).should_be_function();
		value_of(blob.toUpperCase()).should_be("MOZILLA123!?");

		blob = Titanium.API.createBlob("mOZILLA123!?");
		value_of(blob.toUpperCase()).should_be("MOZILLA123!?");

		blob = Titanium.API.createBlob("mO   ZILLA123!?");
		value_of(blob.toUpperCase()).should_be("MO   ZILLA123!?");

		blob = Titanium.API.createBlob("1234567890-=!@#$%^&*()_+");
		value_of(blob.toUpperCase()).should_be("1234567890-=!@#$%^&*()_+");
	},
	test_blob_concat: function()
	{
		var blob = Titanium.API.createBlob("Moz");
		var blob2 = Titanium.API.createBlob("illa");
		value_of(blob.concat).should_be_function();
		value_of(blob.concat("illa")).should_be("Mozilla");
		value_of(blob.concat("illa", " 123", "456")).should_be("Mozilla 123456");
		value_of(blob.concat(blob2)).should_be("Mozilla");
		
		value_of(Titanium.API.createBlob("Moz",blob2)).should_be("Mozilla");
	},
	test_blob_from_binary: function()
	{
		var blob = Titanium.API.createBlob(0xDE, 0xAD, 0xBE, 0xEF);
		value_of(blob.length).should_be(4);
		
		value_of(blob.byteAt(0)).should_be(0xDE);
		value_of(blob.byteAt(1)).should_be(0xAD);
		value_of(blob.byteAt(2)).should_be(0xBE);
		value_of(blob.byteAt(3)).should_be(0xEF);
	}
});
