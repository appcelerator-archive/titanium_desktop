require 'foo'
builder_path = Titanium.API.getApplication().getResourcesPath() + '/builder/lib'
$LOAD_PATH << builder_path
require 'builder/lib/builder'
require 'markaby/lib/markaby'
require 'ostruct'

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
	
	
