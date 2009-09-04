from foo import Foo
import types

def external_document():
	return document.getElementById('a')
	
def require_file_module():
	f = Foo()
	return f.bar()
	
def require_sub_file_module():
	f = Foo()
	return f.yum()
	
def test_window_global():
	return my_global_foo('suck ass')
	
def what_is_love():
	f = Foo()
	return f.what_is_love(my_global_var)

def test_document_title():
	return document.title

def test_external_module():
	return Foo().go_google()
	
def test_type_string():
	return 'i am %s' % 'string'	

def test_type_int():
	return 1
	
def test_type_float():
	return 1.1

def test_type_long():
	return 10**100 #<= google

def test_type_scientific_notation_large():
	return 1.7e19

def test_type_scientific_notation_small():
	return 3.21e-13

def test_type_boolean_false():
	return False

def test_type_boolean_true():
	return True
	
def test_type_map():
	return {'a':'b'}
	
def test_type_tuple():
	return ('a','b')

def test_type_none():
	return None
	
def test_type_dict():
	return dict(one=2,two=3)
	
def test_type_function():
	return test_type_dict
	
def test_type_anonymous_function():
	def anonymous(*args):
		return 'foobar'
	return anonymous

# tests for conversion from JS into Python
def test_js_type_string(t):
	return type(t) == types.StringType
	
def test_js_type_int(t):
	return type(t) == types.IntType

def test_js_type_function(t):
	return type(t) == types.FunctionType

def test_js_type_float(t):
	return type(t) == types.FloatType

def test_js_type_list(t):
	return type(t) == types.ListType

def test_js_type_klist(t):
	return type(t).__name__ == 'KList'

def test_js_type_kmethod(t):
	return type(t).__name__ == 'KMethod'

def test_js_type_kobject(t):
	return type(t).__name__ == 'KObject'

def test_js_type_dict(t):
	return type(t) == types.DictType

def test_js_type_bool(t):
	return type(t) == types.BooleanType
	
def test_js_type_none(t):
	return type(t) == types.NoneType

def get_python_list():
	return [1, 2, 3]

def js_array_sum(jsarray):
	sum = 0
	for x in jsarray:
		sum = sum + x
	return sum

def get_empty_python_object():
	class BlankObject(object):
		pass

	return BlankObject()

def get_empty_python_dict():
	return {}

def get_empty_python_list():
	return []
