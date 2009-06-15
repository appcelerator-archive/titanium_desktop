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
 * @tiapi(method=True,name=Notification.Notification.setTitle,since=0.2) Sets the title of a Notification object
 * @tiarg(for=Notification.Notification.setTitle,name=title,type=string) the title of the Notification object
 */
  this.setTitle = function(value)
  {
    title = value;
  };

/**
 * @tiapi(method=True,name=Notification.Notification.setMessage,since=0.2) Sets the message of a Notification object
 * @tiarg(for=Notification.Notification.setMessage,name=message,type=string) the message of the Notification object
 */
  this.setMessage = function(value)
  {
    message = value;
  };

/**
 * @tiapi(method=True,name=Notification.Notification.setIcon,since=0.2) Sets the icon of a Notification object
 * @tiarg(for=Notification.Notification.setIcon,name=icon,type=string) the path of the icon for the Notification object
 */
  this.setIcon = function(value)
  {
    icon = value;
  };

/**
 * @tiapi(method=True,name=Notification.Notification.setDelay,since=0.2) Sets the delay time before a Notification object is displayed
 * @tiarg(for=Notification.Notification.setDelay,name=delay,type=integer) the delay time in milliseconds
 */
  this.setDelay = function(value)
  {
    notificationDelay = value;
  };

/**
 * @tiapi(method=True,name=Notification.Notification.setCallback,since=0.2) Sets the callback function fired when a Notification object is clicked
 * @tiarg(for=Notification.Notification.setCallback,name=callback,type=function) the callback function
 */
  this.setCallback = function(value)
  {
	  callback = value;
  };

/**
 * @tiapi(method=True,name=Notification.Notification.show,since=0.2) Displays the Notification object
 * @tiarg(for=Notification.Notification.show,name=animate,type=boolean) whether or not to animate the notification, defaults to true
 * @tiarg(for=Notification.Notification.show,name=autohide,type=boolean) whether or not to automatically hide the notification, defaults to true
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
    mywindow.setURL('app://tinotification.html?title='+encodeURIComponent(title)+'&message='+encodeURIComponent(message)+'&icon='+encodeURIComponent(icon));
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
 * @tiapi(method=True,name=Notification.Notification.hide,since=0.2) Hides a notification object
 * @tiarg(for=Notification.Notification.hide,name=animate,type=boolean) whether or not to animate the notification, defaults to true
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
 * @tiapi(method=True,name=Notification.createNotification,since=0.2) Creates a new Notification object
 * @tiarg(for=Notification.createNotification,name=window,type=object,optional=true) the window object to use
 * @tiresult(for=Notification.createNotification,type=object) a Notification object
 */
Titanium.Notification = {
	createNotification : function(window) {
		return new TitaniumNotification(window);
	}
};