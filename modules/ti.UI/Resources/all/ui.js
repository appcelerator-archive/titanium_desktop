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
	Titanium.JSON = JSON;
})();
 
