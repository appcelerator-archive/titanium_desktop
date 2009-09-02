/*!(c) 2008-2009 Appcelerator, Inc. http://appcelerator.org
 * Licensed under the Apache License, Version 2.0. Please visit
 * http://license.appcelerator.com for full copy of the License.
 **/

// A collection of JS patches for various UI functionality in Titanium

//
// execute in anonymous function block so now variables leak into the
// global scope
//
(function()
{
	// ensure that the window.opener property is set when we open a native 
	// window in the same domain as this window (assuming this window is a child)
	if (Titanium.UI.getCurrentWindow().getParent())
	{
		var d = Titanium.UI.getCurrentWindow().getParent().window.document;
		// make sure in the same domain
		if (d.domain == document.domain)
		{
			window.opener = Titanium.UI.getCurrentWindow().getParent().window;
		}
	}
	
	window.onload=function()
	{
		// Add app:// support to jquery's http success function
		if (window.jQuery) 
		{
			var originalHttpSuccess = jQuery.httpSuccess;
			jQuery.extend({
				httpSuccess: function(r){
					if (location.protocol == 'app:' && r.status === 0) {
						return true;
					}
					return originalHttpSuccess.call(this,r);
				}
			}); 
		}
		
		// Add app:// support to MooTool's Request class
		if (window.MooTools && typeof(Request) == "function")
		{
			Request.prototype.isSuccess = function()
			{
				return (((this.status >= 200) && (this.status < 300))
					|| (!!(window.Titanium) && (this.status == 0)));
			};
		}
		
		// adjust background transparency for window if needed
		if(Titanium.platform == "win32") {
			if(Titanium.UI.currentWindow.getTransparency() < 1) {
				var c = Titanium.UI.currentWindow.getTransparencyColor();
				document.body.style.background='#' + c;
			}
		}

		// append the platform (osx, linux, win32) to the body so we can dynamically
		// use platform specific CSS such as body.win32 div { } 
		var cn = (document.body.className || '');
		document.body.className =  cn + (cn ? ' ': '') + Titanium.platform;

		//
		// insert our user specific stylesheet in a generic way
		//
		var link = document.createElement('link');
		link.setAttribute('rel','stylesheet');
		link.setAttribute('href','ti://tiui/default.css');
		link.setAttribute('type','text/css');


		var headNodes = document.getElementsByTagName("head");
		if (headNodes && headNodes.length > 0)
		{
			var head = headNodes[0];
			// if we have children, insert at the top
			if (head.childNodes.length > 0)
			{
				head.insertBefore(link,head.childNodes[0]);
			}
			else
			{
				head.appendChild(link);
			}
		}
		else
		{
			// else we don't have a <head> element, just insert
			// in the body at the top
			if (document.body.childNodes.length > 0)
			{
				document.body.insertBefore(link,document.body.childNodes[0]);
			}
			else
			{
				document.body.appendChild(link);
			}
		}
	};

	
	//
	// override console.log to also send into our API logger
	//
	// patch from cb1kenobi
	function replaceMethod(obj, methodName, newMethod)
	{
		var originalMethodName = methodName + "_orig";
		obj[originalMethodName] = obj[methodName];
		var fn = function()
		{
			newMethod.call(arguments);
			obj[originalMethodName].apply(obj, arguments);
		};
		obj[methodName] = fn;
	}
	replaceMethod(console, "debug", function(msg) { Titanium.API.debug(msg); });
	replaceMethod(console, "log", function(msg) { Titanium.API.log(msg); });
	replaceMethod(console, "info", function(msg) { Titanium.API.info(msg); });
	replaceMethod(console, "warn", function(msg) { Titanium.API.warn(msg); });
	replaceMethod(console, "error", function(msg) { Titanium.API.error(msg); });

	//
	// UI Dialog class
	//
	/**
	 * @tiapi(property=True,name=UI.Window.isDialog,since=0.4) true if this window is a UI Dialog
	 */
	Titanium.UI.getCurrentWindow().isDialog = function()
	{
		return Titanium.UI.getCurrentWindow()._isDialog;
	}
	Titanium.UI.getCurrentWindow()._isDialog = false;

	/**
	 * @tiapi(method=True,name=UI.showDialog,since=0.4) create a UI dialog
	 * @tiarg(for=UI.showDialog,name=params,type=Object) options to pass in to create window
	 * @tiresult(for=UI.showDialog,type=UI.Dialog) dialog instance
	 */
	Titanium.UI.showDialog = function(params)
	{
		Titanium.API.debug("creating dialog with url: "+params.url);
		var dialogWindow = Titanium.UI.createWindow(params);

		dialogWindow._dialogResult = null;
		dialogWindow._dialogParameters = params.parameters || {};
		dialogWindow._isDialog = true;
		dialogWindow._dialogCloseCallback = params.onclose || null;

		/**
		 * @tiapi(method=True,name=UI.Dialog.getDialogResult,since=0.4) get results from UI dialog
		 */
		dialogWindow.getDialogResult = function()
		{
			return dialogWindow._dialogResult;
		}

		/**
		 * @tiapi(method=True,name=UI.Dialog.getDialogParameter,since=0.4)
		 * @tiapi Get an incoming UI dialog parameter
		 * @tiarg[string, name] Name of the parameter
		 * @tiarg[string, any] Default value of the parameter
		 * @tiresult[any] The parameter
		 */
		dialogWindow.getDialogParameter = function(name, defaultValue)
		{
			var v = dialogWindow._dialogParameters[name];
			return v || defaultValue;
		};


		if (dialogWindow._dialogCloseCallback != null)
		{
			var originalClose = dialogWindow.close;
			dialogWindow.close = function(result)
			{
				var onclose = dialogWindow._dialogCloseCallback;
				dialogWindow._dialogResult = result || null;	
				originalClose();
				onclose(dialogWindow._dialogResult);
			}
		}

		//var get_result_text = function(result) { var text = "\n"; for (var x in result) { text += x + ": " + result[x] + "\n"; } return text; }
		dialogWindow.open();
		dialogWindow.show();
		return dialogWindow;
	};
	
	/**
	 * @tiapi(method=True,name=JSON.stringify,since=0.4) Convert a Javascript object to a JSON string
	 * @tiarg(for=JSON.stringify,name=object,type=Object) Javascript object to convert
	 * @tiresult(for=JSON.stringify,type=String) Returns the string representation of the object in JSON format
	 */
	
	/**
	 * @tiapi(method=True,name=JSON.parse,since=0.4) Convert a JSON string to a Javascript object
	 * @tiarg(for=JSON.parse,name=json,type=String) JSON string to convert
	 * @tiresult(for=JSON.parse,type=Object) Returns the object representation of the string 
	 */
	
	
	/*
	    http://www.JSON.org/json2.js
	    2009-04-16

	    Public Domain.

	    NO WARRANTY EXPRESSED OR IMPLIED. USE AT YOUR OWN RISK.

	    See http://www.JSON.org/js.html

	    This is a reference implementation. You are free to copy, modify, or
	    redistribute.

	    This code should be minified before deployment.
	    See http://javascript.crockford.com/jsmin.html

	    USE YOUR OWN COPY. IT IS EXTREMELY UNWISE TO LOAD CODE FROM SERVERS YOU DO
	    NOT CONTROL.
	*/

	/*jslint evil: true */

	// Create a JSON object only if one does not already exist. We create the
	// methods in a closure to avoid creating global variables.

	if (!Titanium.JSON) {
		Titanium.JSON = {};
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
					 f(this.getUTCDate())	  + 'T' +
					 f(this.getUTCHours())	 + ':' +
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
			meta = {	// table of character substitutions
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

			var i,		  // The loop counter.
				k,		  // The member key.
				v,		  // The member value.
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

		if (typeof Titanium.JSON.stringify !== 'function') {
			Titanium.JSON.stringify = function (value, replacer, space) {

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
					throw new Error('Titanium.JSON.stringify');
				}

	// Make a fake root object containing our value under the key of ''.
	// Return the result of stringifying the value.

				return str('', {'': value});
			};
		}


	// If the JSON object does not yet have a parse method, give it one.

		if (typeof Titanium.JSON.parse !== 'function') {
			Titanium.JSON.parse = function (text, reviver) {

	// The parse method takes a text and an optional reviver function, and returns
	// a JavaScript value if the text is a valid JSON text.

				var j;

				function walk(holder, key) {

	// The walk method is used to recursively walk the resulting structure so
	// that modifications can be made.

					var k, v, value = holder[key];
					if (value && typeof value === 'object') {
						for (k in value) {
							if (Object.hasOwnProperty.call(value, k)) {
								v = walk(value, k);
								if (v !== undefined) {
									value[k] = v;
								} else {
									delete value[k];
								}
							}
						}
					}
					return reviver.call(holder, key, value);
				}


	// Parsing happens in four stages. In the first stage, we replace certain
	// Unicode characters with escape sequences. JavaScript handles many characters
	// incorrectly, either silently deleting them, or treating them as line endings.

				cx.lastIndex = 0;
				if (cx.test(text)) {
					text = text.replace(cx, function (a) {
						return '\\u' +
							('0000' + a.charCodeAt(0).toString(16)).slice(-4);
					});
				}

	// In the second stage, we run the text against regular expressions that look
	// for non-JSON patterns. We are especially concerned with '()' and 'new'
	// because they can cause invocation, and '=' because it can cause mutation.
	// But just to be safe, we want to reject all unexpected forms.

	// We split the second stage into 4 regexp operations in order to work around
	// crippling inefficiencies in IE's and Safari's regexp engines. First we
	// replace the JSON backslash pairs with '@' (a non-JSON character). Second, we
	// replace all simple value tokens with ']' characters. Third, we delete all
	// open brackets that follow a colon or comma or that begin the text. Finally,
	// we look to see that the remaining characters are only whitespace or ']' or
	// ',' or ':' or '{' or '}'. If that is so, then the text is safe for eval.

				if (/^[\],:{}\s]*$/.
	test(text.replace(/\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g, '@').
	replace(/"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g, ']').
	replace(/(?:^|:|,)(?:\s*\[)+/g, ''))) {

	// In the third stage we use the eval function to compile the text into a
	// JavaScript structure. The '{' operator is subject to a syntactic ambiguity
	// in JavaScript: it can begin a block or an object literal. We wrap the text
	// in parens to eliminate the ambiguity.

					j = eval('(' + text + ')');

	// In the optional fourth stage, we recursively walk the new structure, passing
	// each name/value pair to a reviver function for possible transformation.

					return typeof reviver === 'function' ?
						walk({'': j}, '') : j;
				}

	// If the text is not JSON parseable, then a SyntaxError is thrown.

				throw new SyntaxError('Titanium.JSON.parse');
			};
		}
	}());

})();
 
