/*!(c) 2008-2010 Appcelerator, Inc. http://appcelerator.org
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
		if (typeof(d)!='undefined' && d.domain == document.domain)
		{
			window.opener = Titanium.UI.getCurrentWindow().getParent().window;
		}
	}

	window.onload=function()
	{
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
			newMethod.apply(null, arguments);
			obj[originalMethodName].apply(obj, arguments);
		};
		obj[methodName] = fn;
	}
	replaceMethod(console, "debug", function(msg) { Titanium.API.debug(msg); });
	replaceMethod(console, "log", function(msg) { Titanium.API.log(msg); });
	replaceMethod(console, "info", function(msg) { Titanium.API.info(msg); });
	replaceMethod(console, "warn", function(msg) { Titanium.API.warn(msg); });
	replaceMethod(console, "error", function(msg) { Titanium.API.error(msg); });

	// Exchange the open() method for a version which ensures that a blank
	// URL maps to app://__blank__ rather than about:blank.
	var _oldOpenFunction = window.open;
	window.open = function()
	{
		var newArgs = Array();
		if (arguments.length < 1 || arguments[0] == "about:blank")
		{
			newArgs[0] = "app://__blank__.html";
		}
		else
		{
			newArgs[0] = arguments[0];
		}

		for (var i = 1; i < arguments.length; i++)
		{
			newArgs[i] = arguments[i];
		}

		return _oldOpenFunction.apply(window, newArgs);
	}

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

		dialogWindow.open();
		dialogWindow.show();
		return dialogWindow;
	};
	
	/**
	 * @tiapi(method=True,name=JSON.stringify,since=0.4) Convert a JavaScript object to a JSON string
	 * @tiarg(for=JSON.stringify,name=object,type=Object) JavaScript object to convert
	 * @tiresult(for=JSON.stringify,type=String) Returns the string representation of the object in JSON format
	 */
	
	/**
	 * @tiapi(method=True,name=JSON.parse,since=0.4) Convert a JSON string to a JavaScript object
	 * @tiarg(for=JSON.parse,name=json,type=String) JSON string to convert
	 * @tiresult(for=JSON.parse,type=Object) Returns the object representation of the string 
	 */
	if (!Titanium.JSON)
		Titanium.JSON = window.JSON;

	// Enable titanium notifications as a fallback when platform
	// native notifications are not available.
	if (Titanium.UI.nativeNotifications == false)
	{
		var notification_windows = 1;

		function TitaniumNotification(window)
		{
			var self = this;
			var width = 300;
			var height = 80;
			var showing = false;
			var hideTimer = null;

			var properties = {
				title: '',
				message: '',
				icon: '',
				callback: null,
				delay: 3000,
			};

			var mywindow = Titanium.UI.mainWindow.createWindow(
			{
				width:width,
				height:height,
				transparentBackground:true,
				usingChrome:false,
				toolWindow:true,
				id:'notification_' + (notification_windows++),
				visible:false,
				topMost:true,
				url:'app://blank'
			});
			mywindow.open();

			this.setTitle = function(value)
			{
				properties.title = value;
			};

			this.setMessage = function(value)
			{
				properties.message = value;
			};

			this.setIcon = function(value)
			{
				properties.icon = value;
			};

			this.setDelay = function(value)
			{
				properties.delay = value;
			};
			this.setTimeout = this.setDelay;

			this.setCallback = function(value)
			{
				properties.callback = value;
			};

			this.show = function(animate,autohide)
			{
				showing = true;
				if (hideTimer)
				{
					window.clearTimeout(hideTimer);
				}
				animate = (animate==null) ? true : animate;
				autohide = (autohide==null) ? true : autohide;
				mywindow.setX(window.screen.availWidth-width-20);
				if (Titanium.platform == "osx" || Titanium.platform == 'linux') {
					mywindow.setY(25);
				} else if (Titanium.platform == "win32") {
					mywindow.setY(window.screen.availHeight-height-10);
				}

				var notificationClicked = function ()
				{
					if (properties.callback)
						properties.callback();
					self.hide();
				};

				mywindow.callback = notificationClicked;
				mywindow.setURL('ti://tiui/tinotification.html?'
				 + 'title=' + encodeURIComponent(properties.title)
				 + '&message=' + encodeURIComponent(properties.message)
				 + '&icon=' + encodeURIComponent(properties.icon));

				mywindow.show();
				if (autohide)
				{
					hideTimer = window.setTimeout(function()
					{
						self.hide();
					},properties.delay + (animate ? 1000 : 0));
				}
			};

			this.hide = function(animate)
			{
				animate = (animate==null) ? true : animate;
				showing = false;
				if (hideTimer)
				{
					window.clearTimeout(hideTimer);
					hideTimer=null;
				}
				mywindow.hide(animate);
				mywindow.getParent().focus();
			};

			this.configure = function(props)
			{
				for (attr in props) properties[attr] = props[attr];
			};
		};

		Titanium.UI.createNotification = function()
		{
			var window = Titanium.UI.mainWindow.getDOMWindow();
			if (!window)
				throw "Unable to get main window DOM!"
			var n = new TitaniumNotification(window);
			if (arguments.length == 1)
				n.configure(arguments[0]);
			return n;
		}
	}
})();
 
