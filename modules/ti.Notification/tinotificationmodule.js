var notification_windows = 1;

function TitaniumNotification(window)
{
	var width = 300, height = 80, notificationDelay = 3000;
	if (Titanium.platform == "win32") {
		height = 80;  
	}
	var showing = false;
	var myid = 'notification_'+(notification_windows++);
	var transparency = .99;
	
	var mywindow = Titanium.UI.mainWindow.createWindow({
		width:width,
		height:height,
		transparency:transparency,
		usingChrome:false,
		id:myid,
		visible:false,
		topMost:true,
		url:'app://blank'
	});
	var self = this;
	var title = '', message = '', icon = '';
	var callback = null;
	var hideTimer = null;

	/**
	 * @tiapi(method=True,name=Notification.Notification.setTitle,since=0.2)
	 * @tiapi Sets the title of a Notification object
	 * @tiarg[String, title] The title of the Notification object
	 */
	this.setTitle = function(value)
	{
		title = value;
	};

	/**
	 * @tiapi(method=True,name=Notification.Notification.setMessage,since=0.2)
	 * @tiapi Sets the message of a Notification object
	 * @tiarg[String, message] The message of the Notification object
	 */
	this.setMessage = function(value)
	{
		message = value;
	};

	/**
	 * @tiapi(method=True,name=Notification.Notification.setIcon,since=0.2)
	 * @tiapi Sets the icon of a Notification object
	 * @tiarg[String, icon] The path of the icon for the Notification object
	 */
	this.setIcon = function(value)
	{
		icon = value;
	};

	/**
	 * @tiapi(method=True,name=Notification.Notification.setDelay,since=0.2)
	 * @tiapi Sets the delay time before a Notification object is displayed
	 * @tiarg[Number, delay] The delay time in milliseconds
	 */
	this.setDelay = function(value)
	{
		notificationDelay = value;
	};

	/**
	 * @tiapi(method=True,name=Notification.Notification.setCallback,since=0.2)
	 * @tiapi Sets the callback function fired when a Notification object is clicked
	 * @tiarg[Function, callback] The callback function
	 */
	this.setCallback = function(value)
	{
		callback = value;
	};

	/**
	 * @tiapi(method=True,name=Notification.Notification.show,since=0.2)
	 * Displays the Notification object
	 * @tiarg[Boolean, animate] Whether or not to animate the notification, defaults to true
	 * @tiarg[Boolean, autoHide] Whether or not to automatically hide the notification, defaults to true
	 */
	this.show = function(animate,autohide)
	{
		if ('Growl' in Titanium && Titanium.Growl.isRunning()) {
			Titanium.Growl.showNotification(title, message, icon, notificationDelay/1000, callback);
			return;
		}

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
			if (callback)
				callback();
			self.hide();
		};

		mywindow.setTransparency(.99);
		mywindow.callback = notificationClicked;
		mywindow.setURL('ti://tinotification/tinotification.html?title='+encodeURIComponent(title)+'&message='+encodeURIComponent(message)+'&icon='+encodeURIComponent(icon));
		mywindow.open();
		mywindow.show();
		if (autohide)
		{
			hideTimer = window.setTimeout(function()
			{
				self.hide();
			},notificationDelay + (animate ? 1000 : 0));
		}
	};

	/**
	 * @tiapi(method=True,name=Notification.Notification.hide,since=0.2)
	 * @tiapi Hides a notification object
	 * @tiarg[Boolean, animate] Whether or not to animate the notification, defaults to true
	 */
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
};

	/**
	 * @tiapi(method=True,name=Notification.createNotification,since=0.2)
	 * @tiapi Creates a new Notification object
	 * @tiarg[UI.UserWindow, window, optional=True] The window object to use
	 * @tiresult[Notification.Notification] a Notification object
	 */
	Titanium.Notification = {
		createNotification : function(window) {
			return new TitaniumNotification(window);
		}
	};
