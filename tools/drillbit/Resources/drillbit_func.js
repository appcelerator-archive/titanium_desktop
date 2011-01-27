/*
    http://www.JSON.org/json2.js
    2009-04-16

    Public Domain.

    NO WARRANTY EXPRESSED OR IMPLIED. USE AT YOUR OWN RISK.

    See http://www.JSON.org/js.html

*/

// Create a JSON object only if one does not already exist. We create the
// methods in a closure to avoid creating global variables.

if (!this.JSON) {
    JSON = {};
}
(function () {

    function f(n) {
        // Format integers to have at least two digits.
        return n < 10 ? '0' + n : n;
    }

    if (typeof Date.prototype.toJSON !== 'function') {

        Date.prototype.toJSON = function (key) {

            return this.getUTCFullYear()   + '-' +
                 f(this.getUTCMonth() + 1) + '-' +
                 f(this.getUTCDate())      + 'T' +
                 f(this.getUTCHours())     + ':' +
                 f(this.getUTCMinutes())   + ':' +
                 f(this.getUTCSeconds())   + 'Z';
        };

        String.prototype.toJSON =
        Number.prototype.toJSON =
        Boolean.prototype.toJSON = function (key) {
            return this.valueOf();
        };
    }

    var cx = /[\u0000\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g,
        escapable = /[\\\"\x00-\x1f\x7f-\x9f\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g,
        gap,
        indent,
        meta = {    // table of character substitutions
            '\b': '\\b',
            '\t': '\\t',
            '\n': '\\n',
            '\f': '\\f',
            '\r': '\\r',
            '"' : '\\"',
            '\\': '\\\\'
        },
        rep;


    function quote(string) {

// If the string contains no control characters, no quote characters, and no
// backslash characters, then we can safely slap some quotes around it.
// Otherwise we must also replace the offending characters with safe escape
// sequences.

        escapable.lastIndex = 0;
        return escapable.test(string) ?
            '"' + string.replace(escapable, function (a) {
                var c = meta[a];
                return typeof c === 'string' ? c :
                    '\\u' + ('0000' + a.charCodeAt(0).toString(16)).slice(-4);
            }) + '"' :
            '"' + string + '"';
    }


    function str(key, holder) {

// Produce a string from holder[key].

        var i,          // The loop counter.
            k,          // The member key.
            v,          // The member value.
            length,
            mind = gap,
            partial,
            value = holder[key];

// If the value has a toJSON method, call it to obtain a replacement value.

        if (value && typeof value === 'object' &&
                typeof value.toJSON === 'function') {
            value = value.toJSON(key);
        }

// If we were called with a replacer function, then call the replacer to
// obtain a replacement value.

        if (typeof rep === 'function') {
            value = rep.call(holder, key, value);
        }

// What happens next depends on the value's type.

        switch (typeof value) {
        case 'string':
            return quote(value);

        case 'number':

// JSON numbers must be finite. Encode non-finite numbers as null.

            return isFinite(value) ? String(value) : 'null';

        case 'boolean':
        case 'null':

// If the value is a boolean or null, convert it to a string. Note:
// typeof null does not produce 'null'. The case is included here in
// the remote chance that this gets fixed someday.

            return String(value);

// If the type is 'object', we might be dealing with an object or an array or
// null.

        case 'object':

// Due to a specification blunder in ECMAScript, typeof null is 'object',
// so watch out for that case.

            if (!value) {
                return 'null';
            }

// Make an array to hold the partial results of stringifying this object value.

            gap += indent;
            partial = [];

// Is the value an array?

            if (Object.prototype.toString.apply(value) === '[object Array]') {

// The value is an array. Stringify every element. Use null as a placeholder
// for non-JSON values.

                length = value.length;
                for (i = 0; i < length; i += 1) {
                    partial[i] = str(i, value) || 'null';
                }

// Join all of the elements together, separated with commas, and wrap them in
// brackets.

                v = partial.length === 0 ? '[]' :
                    gap ? '[\n' + gap +
                            partial.join(',\n' + gap) + '\n' +
                                mind + ']' :
                          '[' + partial.join(',') + ']';
                gap = mind;
                return v;
            }

// If the replacer is an array, use it to select the members to be stringified.

            if (rep && typeof rep === 'object') {
                length = rep.length;
                for (i = 0; i < length; i += 1) {
                    k = rep[i];
                    if (typeof k === 'string') {
                        v = str(k, value);
                        if (v) {
                            partial.push(quote(k) + (gap ? ': ' : ':') + v);
                        }
                    }
                }
            } else {

// Otherwise, iterate through all of the keys in the object.

                for (k in value) {
                    if (Object.hasOwnProperty.call(value, k)) {
                        v = str(k, value);
                        if (v) {
                            partial.push(quote(k) + (gap ? ': ' : ':') + v);
                        }
                    }
                }
            }

// Join all of the member texts together, separated with commas,
// and wrap them in braces.

            v = partial.length === 0 ? '{}' :
                gap ? '{\n' + gap + partial.join(',\n' + gap) + '\n' +
                        mind + '}' : '{' + partial.join(',') + '}';
            gap = mind;
            return v;
        }
    }

// If the JSON object does not yet have a stringify method, give it one.

    if (typeof JSON.stringify !== 'function') {
        JSON.stringify = function (value, replacer, space) {

// The stringify method takes a value and an optional replacer, and an optional
// space parameter, and returns a JSON text. The replacer can be a function
// that can replace values, or an array of strings that will select the keys.
// A default replacer method can be provided. Use of the space parameter can
// produce text that is more easily readable.

            var i;
            gap = '';
            indent = '';

// If the space parameter is a number, make an indent string containing that
// many spaces.

            if (typeof space === 'number') {
                for (i = 0; i < space; i += 1) {
                    indent += ' ';
                }

// If the space parameter is a string, it will be used as the indent string.

            } else if (typeof space === 'string') {
                indent = space;
            }

// If there is a replacer, it must be a function or an array.
// Otherwise, throw an error.

            rep = replacer;
            if (replacer && typeof replacer !== 'function' &&
                    (typeof replacer !== 'object' ||
                     typeof replacer.length !== 'number')) {
                throw new Error('JSON.stringify');
            }

// Make a fake root object containing our value under the key of ''.
// Return the result of stringifying the value.

            return str('', {'': value});
        };
    }
}());

TitaniumTest = 
{
	currentTest:null,
	results:[],
	tests:[],
	success:0,
	failed:0,
	totalAssertions:0,
	
	runningTest:function(suite,name)
	{
		Titanium.App.stdout('DRILLBIT_TEST: '+suite+','+name);
		Titanium.API.debug('DRILLBIT_TEST: '+suite+','+name);
	},
	
	assertion:function(subject)
	{
		Titanium.App.stdout('DRILLBIT_ASSERTION: ' + TitaniumTest.currentTest + "," + subject.lineNumber);
		TitaniumTest.totalAssertions++;
	},
	
	testPassed:function(name, lineNumber)
	{
		this.success++;
		this.results.push({
			name:name,
			passed:true,
			message: "Success",
			lineNumber: lineNumber
		});
		Titanium.App.stdout("DRILLBIT_PASS: "+name);
		Titanium.API.debug("DRILLBIT_PASS: "+name);
		TitaniumTest.run_next_test();
	},
	
	testFailed:function(name,e)
	{
		this.failed++;
		this.results.push({
			name:name,
			passed:false,
			lineNumber:e.line,
			message:e.message || String(e)
		});
		
		Titanium.App.stdout("DRILLBIT_FAIL: "+name+","+e.line+" --- "+String(e).replace("\n","\\n"));
		Titanium.API.debug("DRILLBIT_FAIL: "+name+","+e.line+" --- "+e);
		TitaniumTest.run_next_test();
	},
	
	complete: function()
	{
		try
		{
			Titanium.API.info("test complete");
			var results_dir = Titanium.API.getApplication().getArgumentValue('results-dir');
			if (results_dir==null)
			{
				Titanium.API.error("INVALID ARGUMENT VALUE FOUND FOR ARG: results-dir");
			}
			var rd = Titanium.Filesystem.getFile(results_dir);
			if (!rd.exists())
			{
				rd.createDirectory(true);
			}
			var f = Titanium.Filesystem.getFile(rd.nativePath(), TitaniumTest.NAME+'.json');
			this.write_results_to_json(f);

			// Only write the failure report HTML if we have failed -- it's very expensive
			var f = Titanium.Filesystem.getFile(rd.nativePath(), TitaniumTest.NAME+'.html');
			this.write_results_to_single_html(f);
			
			var f = Titanium.Filesystem.getFile(rd.nativePath(), "results.html");
			this.write_results_to_results_html(f);
		}
		catch(e)
		{
			Titanium.API.error("Exception on completion: "+e);
		}
		Titanium.App.exit(0);
	},
	
	write_results_to_json: function(f)
	{
		var data = {
			'results':this.results,
			'count':this.results.length,
			'success':this.success,
			'failed':this.failed,
			'assertions':this.assertions
		};
		f.write(JSON.stringify(data));
	},

	write_results_to_single_html: function(f)
	{
		var text = [];
		text.push("<html><body>");
		text.push("<style>.failed{background-color:yellow;} body {background-color: white;}</style>");
		
		this.get_results_html(text);
		
		text.push("</body></html>");
		f.write(text.join("\n"));
	},
	
	write_results_to_results_html: function(f)
	{
		var text = [];
		this.get_results_html(text);
		
		f.write(text.join("\n"), true);
	},
	
	get_results_html: function(text)
	{
		text.push("<table>");

		text.push("<tr>");
		text.push("<td><b>Test name</b></td>");
		text.push("<td><b>Passed?</b></td>");
		text.push("<td><b>Line number</b></td>");
		text.push("<td><b>Message</b></td>");
		text.push("</tr>");

		var failed = false, failedLines = [];
		for (var i = 0; i < this.results.length; i++)
		{
			var lineno = this.results[i].lineNumber;
			if (!this.results[i].passed)
			{
				failed = true;
				failedLines.push(lineno);
			}

			text.push("<tr>");
			text.push("<td>" + this.results[i].name + "</td>");
			text.push("<td>" + this.results[i].passed + "</td>");
			text.push('<td><a href="#l' + lineno + '">' + lineno + "</a></td>");
			text.push("<td>" + this.results[i].message + "</td>");
			text.push("</tr>");
		}
		text.push("</table>");

		if (failed)
		{
			var app = Titanium.API.getApplication();
			//var script = Titanium.Filesystem.getFile(
			//app.getResourcesPath(), "userscripts", TitaniumTest.NAME + "_driver.js");
			var scriptText = Titanium.Filesystem.getFile(TitaniumTest.SOURCE).read();
			var lines = scriptText.toString().split("\n");

			text.push('<table style="font-family: monospace; font-size: 10pt;">');
			for (var i = 0; i < lines.length; i++)
			{
				var num = i + 1;
				var line = lines[i].replace(/&/g,'&amp;').replace(/>/g,'&gt;').replace(/</g,'&lt;').replace(/"/g,'&quot;');
				var failed = failedLines.indexOf(num)!=-1;
				var cls = failed ? 'failed':'passed';
				text.push('<tr>');
				text.push('<td class="'+cls+'"><a name="l' + num + '">' + num + '</a></td>');
				text.push('<td class="'+cls+'">' + line + '</td>');
				text.push('</tr>');
			}
			text.push("</table>");
		}
	},
	
	on_complete: function()
	{
		this.complete();
	},
	
	run_next_test:function()
	{
		Titanium.API.info("test run_next_test "+this.tests.length);
		if (this.tests.length == 0)
		{
			this.on_complete();
		}
		else
		{
			var t = this.tests.shift();
			t();
		}
	}
};

TitaniumTest.gscope = {};
TitaniumTest.currentSubject = null;

function value_of(obj)
{
	var subject = new TitaniumTest.Subject(obj);
	TitaniumTest.currentSubject = subject;
	return subject;
}

TitaniumTest.Error = function(message,line)
{
	this.message = message;
	this.line = line;
};

TitaniumTest.Error.prototype.toString = function()
{
	return this.message;
};

TitaniumTest.Subject = function(target) {
	this.target = target;
	this.lineNumber = 0;
};

TitaniumTest.Subject.prototype.toString = function()
{
	return 'Subject[target='+this.target+',line='+this.lineNumber+']';
};

TitaniumTest.Scope = function(name) {
	this._testName = name;
	this._completed = false;
	// copy in the global scope
	for (var p in TitaniumTest.gscope)
	{
		this[p] = TitaniumTest.gscope[p];
	}
}

TitaniumTest.Scope.prototype.passed = function()
{
	if (!this._completed)
	{
		this._completed = true;
		if (TitaniumTest.currentSubject)
		{
			TitaniumTest.testPassed(this._testName,TitaniumTest.currentSubject.lineNumber);
		}
		else
		{
			TitaniumTest.testPassed(this._testName,-1);
		}
		TitaniumTest.currentSubject = null;
	}
}

TitaniumTest.Scope.prototype.failed = function(ex)
{
	if (!this._completed)
	{
		this._completed = true;
		TitaniumTest.testFailed(this._testName,ex);
		TitaniumTest.currentSubject = null;
	}
}

TitaniumTest.Subject.prototype.should_be = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target != expected)
	{
		throw new TitaniumTest.Error('should be: "'+expected+'", was: "'+this.target+'"',lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_not_be = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target == expected)
	{
		throw new TitaniumTest.Error('should not be: '+expected+', was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_not_be_null = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target === null)
	{
		throw new TitaniumTest.Error('should not be null, was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_not_be_undefined = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target === undefined)
	{
		throw new TitaniumTest.Error('should not be undefined, was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_exactly = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target !== expected)
	{
		throw new TitaniumTest.Error('should be exactly: '+expected+', was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_null = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target !== null)
	{
		throw new TitaniumTest.Error('should be null, was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_string = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (typeof this.target !== 'string')
	{
		throw new TitaniumTest.Error('should be string, was: '+typeof(this.target),lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_undefined = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target !== undefined)
	{
		throw new TitaniumTest.Error('should be undefined, was: '+this.target,lineNumber);
	}
};


TitaniumTest.Subject.prototype.should_be_function = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (typeof(this.target) != 'function')
	{
		throw new TitaniumTest.Error('should be a function, was: '+typeof(this.target),lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_object = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (typeof(this.target) != 'object')
	{
		throw new TitaniumTest.Error('should be a object, was: '+typeof(this.target),lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_number = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (typeof(this.target) != 'number')
	{
		throw new TitaniumTest.Error('should be a number, was: '+typeof(this.target),lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_boolean = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (typeof(this.target) != 'boolean')
	{
		throw new TitaniumTest.Error('should be a boolean, was: '+typeof(this.target),lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_true = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target!==true)
	{
		throw new TitaniumTest.Error('should be true, was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_false = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target!==false)
	{
		throw new TitaniumTest.Error('should be false, was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_zero = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target!==0)
	{
		throw new TitaniumTest.Error('should be 0 (zero), was: '+this.target+' ('+typeof(this.target)+')',lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_array = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	// better way to check? we need to support our duck-typing too..
	if (this.target.constructor != Array)
	{
		throw new TitaniumTest.Error('should be an array, was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_contain = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target.indexOf(expected)==-1)
	{
		throw new TitaniumTest.Error('should contain: '+expected+', was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_one_of = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (expected.indexOf(this.target)==-1)
	{
		throw new TitaniumTest.Error('should contain one of: ['+expected.join(",")+'] was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_match_array = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target.length && expected.length && this.target.length == expected.length) {
		for (var i = 0; i < expected.length; i++) {
			if (expected[i] != this.target[i]) {
				throw new TitaniumTest.Error('element ' + i + ' should be: '+expected[i]+' was: '+this.target[i],lineNumber);
			}
		}
	}
	else {
		throw new TitaniumTest.Error('array lengths differ, expected: '+expected+', was: '+this.target,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_greater_than = function(expected, lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target <= expected)
	{
		throw new TitaniumTest.Error('should be greater than, was ' + this.target + ' <= ' + expected,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_less_than = function(expected, lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target >= expected)
	{
		throw new TitaniumTest.Error('should be less than, was ' + this.target + ' >= ' + expected,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_greater_than_equal = function(expected, lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target < expected)
	{
		throw new TitaniumTest.Error('should be greater than equal, was ' + this.target + ' < ' + expected,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_be_less_than_equal = function(expected, lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (this.target > expected)
	{
		throw new TitaniumTest.Error('should be greater than, was ' + this.target + ' > ' + expected,lineNumber);
	}
};

TitaniumTest.Subject.prototype.should_throw_exception = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (typeof(this.target) == 'function')
	{
		try {
			this.target();
		} catch (e) { return; }
		throw new TitaniumTest.Error("should throw exception, but didn't",lineNumber);
	}
	else throw new TitaniumTest.Error("should throw exception, but target isn't a function",lineNumber);
};

TitaniumTest.Subject.prototype.should_not_throw_exception = function(expected,lineNumber)
{
	this.lineNumber = lineNumber;
	TitaniumTest.assertion(this);
	if (typeof(this.target) == 'function')
	{
		try {
			this.target();
		} catch (e) { 
			throw new TitaniumTest.Error("should not throw exception, but did",lineNumber);	
		}
	}
	else throw new TitaniumTest.Error("should not throw exception, but target isn't a function",lineNumber);
};