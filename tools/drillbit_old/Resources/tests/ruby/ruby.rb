require 'foo'
$orig_include_path = $LOAD_PATH
builder_path = Titanium.API.getApplication().getResourcesPath() + '/builder/lib'
$LOAD_PATH << builder_path
require 'builder/lib/builder'
require 'markaby/lib/markaby'
require 'ostruct'
require 'rss'

def external_document
	document.getElementById 'a'
end

def require_file_module
  f = Foo.new
  f.bar
end

def require_sub_file_module
  f = Foo.new
  f.yum
end

def test_window_global
  my_global_foo 'suck ass'
end

def what_is_love?
  f = Foo.new
  f.what_is_love?(my_global_var)
end

def test_document_title
  return document.title
end

def test_gem
   mab = Markaby::Builder.new
   mab.html do
     head { title "Boats.com" }
   end
   mab.to_s
end

def test_type_string
	return 'i am string'
end

def test_type_symbol(stringbol)
	return stringbol.to_sym
end

def test_type_float
  1.1
end

def test_type_long
  10**100 #<= google
end

def test_type_scientific_notation_large
  1.7e19
end

def test_type_scientific_notation_small
  3.21e-13
end

def test_type_int
	1
end
	
def test_type_boolean_false
	false
end

def test_type_boolean_true
	true
end
	
def test_type_map
	{'a'=>'b'}
end

def test_get_symbolly_hash
	# We want more than 1/2 of the keys to be symbols
	# for new keys to be treated like symbols.
	return {
		:a => 'a',
		:one => 'b',
		:two => 'c',
		:three => 'd',
		:symbol => 'bully',
		:another_symbol => 'wooly',
		'not_a_symbol' => 'foo'
	}
end

def test_get_less_symbolly_hash
	# We want more than 1/2 of the keys to be symbols
	# for new keys to be treated like symbols.
	return {
		'a' => 'a',
		'one' => 'b',
		'two' => 'c',
		'three' => 'd',
		'four' => 'd',
		'five' => 'd',
		:symbol => 'bully',
		:another_not_symbol => 'wooly',
		'not_a_symbol' => 'foo'
	}
end

def test_symbolly_hash(hash)
	if hash.has_key?('fromjs')
		return false
	elsif hash.has_key?(:fromjs)
		return true
	else
		return false
	end
end

def test_less_symbolly_hash(hash)
	if hash.has_key?(:fromjs)
		return false
	elsif hash.has_key?('fromjs')
		return true
	else
		return false
	end
end

def test_type_tuple
	OpenStruct.new(:one=>2,:two=>3)
end

def test_type_struct
	customer = Struct.new(:name,:address)
	customer.new("jeff","mtn view")
end

def test_type_nil
	nil
end
	
def test_type_array
	[1,2,3]
end
	
def test_type_function
	method(:test_type_array)
end

def test_type_proc
  Proc.new { 'hello world' }
end

# tests for conversion from JS into Ruby
def test_js_type_string(t)
	return t.class == String
end
	
def test_js_type_int(t)
	return t.class == Fixnum
end
	
def test_js_type_function(t)
	return t.class == Method
end

def test_js_type_float(t)
	return t.class == Float
end

def test_js_type_list(t)
	return t.class == Array
end

def test_js_type_klist(t)
	return t.class.name == 'RubyKList'
end

def test_js_klist_elements(t)
	return t[0] == 1 && t[1] == 2 && t[2] == 3
end

def test_js_type_kobject(t)
	return t.class.name == 'RubyKObject'
end
def test_js_type_kmethod(t)
	return t.class.name == 'RubyKMethod'
end

def test_js_type_dict(t)
	return t.class == Hash
end

def test_js_type_true_bool(t)
	return t == true
end

def test_js_type_false_bool(t)
	return t == false
end
	
def test_js_type_nil(t)
	return t.class == NilClass
end

def test_rubykobject_respond_to(o)
	if not(o.respond_to?(:sheep))
		return "Oops: Did not respond to sheep"
	end
	if not(o.respond_to?(:cow))
		return "Oops: Did not respond to cow"
	end
	if not(o.respond_to?(:phillip))
		return "Oops: Did not respond to phillip"
	end
	if o.respond_to?(:undef)
		return "Oops: Responded to undef"
	end
	if o.respond_to?(:bob)
		return "Oops: Responded to bob"
	end
	return ""
end

def test_rubykobject_method_missing_exception(o)
	exception = ""
	begin
		o.method_missing(:blahblah)
		exception = ":blahblah did not throw an Exception"
	rescue NoMethodError
		1 + 1
	rescue 
		exception = ":blahblah did not throw a NoMethodError"
	end

	if (exception != "")
		return exception
	end

	exception = ""
	begin
		o.method_missing(:cow)
	rescue NoMethodError
		exception = ":cow threw a NoMethodError"
	rescue 
		exception = ":cow threw some unknown error: " + $!
	end
	return exception
end

def test_rubyklist_length(l, length)
	if l.length == length
		return ""
	else
		return "Length should have been " + length + " but was " + l.length
	end

	# Test invalid argument handling with the length method
	exception = ""
	begin
		l.length("what, what")
		exception = "length() did not throw an exception"
	rescue ArgumentError
		exception = ""
	rescue
		exception = "length() hrew an unknown exception"
	end
end

def test_rubyklist_each(l, *list_items)
	length = l.length
	exp_length = list_items.length
	if length != exp_length
		return "Length should have been #{exp_length} but was #{length}"
	end

	i = 0
	l.each { |item|
		if (item != list_items[i])
			return "Item #{i} should have been #{list_items[i].inspect} but was #{item.inspect}"
		end
		i = i + 1
	}
	return ""
end

def get_ruby_proc()
	p = Proc.new { |x| x + "foo!" }
	return p
end

def get_ruby_proc_arity(arity)
	if (arity == 0)
		p = Proc.new { "|" }
	elsif (arity == 1)
		p = Proc.new { |x| x + "|" }
	elsif (arity == 2)
		p = Proc.new { |x, y| x + "|" + y + "|" }
	elsif (arity == 3)
		p = Proc.new { |x, y, z| x + "|" + y + "|" + z + "|" }
	elsif (arity == -1)
		p = Proc.new { |x, *y| x + "|" + y.join("|") + "|" }
	elsif (arity == -2)
		p = Proc.new { |x, y, *z| x + "|" + y + "|" + z.join("|") + "|" }
	end

	return p
end

def get_empty_ruby_object()
	return Object.new
end

def get_empty_ruby_list()
	return []
end

def get_empty_ruby_hash()
	#return {} 
	return Object.new
end

def get_include_path()
	return $orig_include_path.join(" : ")
end

def api_is_there()
	return defined?(RSS) != nil
end

def test_rb_collect(input)
    return input.collect {|x| x * 2}
end

def test_rb_map(input)
    return input.map {|x| x * 2}
end
