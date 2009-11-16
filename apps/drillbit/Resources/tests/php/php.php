
function external_document()
{
	global $document;
	return $document->getElementById('a');
}

function test_window_global()
{
	global $window;
	return $window->my_global_foo('suck ass');
}

function test_document_title()
{
	global $document;
	return $document->title;
}

// tests for conversion from JS into PHP
function test_js_type_string($t)
{
	return is_string($t);
}

function test_js_type_int($t)
{
	return is_int($t);
}

function test_js_type_function($t)
{
	return is_callable($t);
}

function test_js_type_float($t)
{
	return is_float($t);
}

function test_js_type_list($t)
{
	return is_array($t);
}

function test_js_type_object($t,$c)
{
	return is_object($t) && get_class($t) == $c;
}

function test_js_klist_elements($t)
{
	return $t[0] == 1 && $t[1] == 2 && $t[2] == 3;
}

function test_js_type_dict($t)
{
	return is_array($t);
}

function test_js_type_true_bool($t)
{
	return $t == true;
}

function test_js_type_false_bool($t)
{
	return $t == false;
}

function test_js_type_null($t)
{
	return $t == null;
}

function test_call_method_prop($o)
{
	return $o->f();
}

function test_call_method_prop_with_arg($o, $arg)
{
	return $o->f2($arg);
}

class Hammer {
	private $privateVariable;
	public $publicVariable;

	function __construct()
	{
		$this->privateVariable = "bar";
		$this->publicVariable = "bar";
	}

	private function privateMethod()
	{
		return "foo";
	}

	public function publicMethod()
	{
		return "foo";
	}
}

function looks_like_a_nail()
{
	return new Hammer();
}

function php_modify_array($myarray)
{
	$myarray[0] = 4;
	$myarray[1] = 5;
	$myarray[2] = 6;
	$myarray[3] = 7;
}

function php_get_anonymous_function()
{
	$anon = function()
	{
		return "blueberry";
	};
	return $anon;
}

function php_get_anonymous_function_one_arg()
{
	$anon = function($one)
	{
		return strtoupper($one);
	};
	return $anon;
}

function php_get_anonymous_function_two_args()
{
	$anon = function($one, $two)
	{
		return strtoupper($one) . strtoupper($two);
	};
	return $anon;
}

function CamelCaseFunctionOne()
{
	return "uno";
}

function camelCaseFunctionTwo()
{
	return "dos";
}

function ALLCAPSFUNCTION()
{
	return "tres";
}

function php_test_mysql()
{
	// try the public ensembl/genome mysql db
	// http://www.ensembl.org/info/data/mysql.html
	if (!function_exists('mysql_connect'))
	{
		return "mysql_connect not defined";
	}

	if (!function_exists('mysql_close'))
	{
		return "mysql_close not defined";
	}

	$link = mysql_connect("ensembldb.ensembl.org:5306", "anonymous", "");
	if (!$link)
	{
		return "mysql_connect did not return a good value.";
	}

	mysql_close($link);
	return true;
}
