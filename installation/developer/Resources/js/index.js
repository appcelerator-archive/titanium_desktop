//
// Vars
//
var TFS = Titanium.Filesystem;
var TiDeveloper  = {};
TiDeveloper.currentPage = 1;
TiDeveloper.init = false;
TiDeveloper.windowFocused = false
TiDeveloper.highestId = 0;

// Global Notification var
var notification = Titanium.Notification.createNotification(window)

// Global DB var
var db = openDatabase("TiDeveloper","1.0");

//
// Generic formatter based a count
//
TiDeveloper.formatCountMessage = function(count,things)
{
	return (count == 0) ? 'You have no '+things+'s' : count == 1 ? 'You have 1 '+things : 'You have ' + count + ' '+things+'s';
};


TiDeveloper.track = function(name,data)
{
	data = (typeof(data)!='undefined') ? swiss.toJSON(data) : null;
	Titanium.Analytics.addEvent(name,data);
};

//
// Track window focus events
//
Titanium.UI.currentWindow.addEventListener(function(event)
{
	if (event == "unfocused")
	{
		TiDeveloper.windowFocused = false;
		$MQ('l:tideveloper.windowFocus',{focus:false});
		
	}
	else if (event == "focused")
	{
		TiDeveloper.windowFocused = true;
		$MQ('l:tideveloper.windowFocus',{focus:true});
	}
});



//
// Get current time
//
TiDeveloper.getCurrentTime = function()
{
	var curDateTime = new Date()
  	var curHour = curDateTime.getHours()
  	var curMin = curDateTime.getMinutes()
  	var curAMPM = "am"
  	var curTime = ""
  	if (curHour >= 12){
    	curHour -= 12
    	curAMPM = "pm"
    }
  	if (curHour == 0) curHour = 12
  	curTime = curHour + ":" + ((curMin < 10) ? "0" : "") + curMin + curAMPM;
  	return curTime;
	
};

// these are not all the TLDs but most of the popular ones
TiDeveloper.TLD = /\.(com|com\.uk|gov|org|net|mil|name|co\.uk|biz|info|edu|tv|mobi)/i;
TiDeveloper.URI_REGEX = /((([hH][tT][tT][pP][sS]?|[fF][tT][pP])\:\/\/)?([\w\.\-]+(\:[\w\.\&%\$\-]+)*@)?((([^\s\(\)\<\>\\\"\.\[\]\,@;:]+)(\.[^\s\(\)\<\>\\\"\.\[\]\,@;:]+)*(\.[a-zA-Z]{2,4}))|((([01]?\d{1,2}|2[0-4]\d|25[0-5])\.){3}([01]?\d{1,2}|2[0-4]\d|25[0-5])))(\b\:(6553[0-5]|655[0-2]\d|65[0-4]\d{2}|6[0-4]\d{3}|[1-5]\d{4}|[1-9]\d{0,3}|0)\b)?((\/[^\/][\w\.\,\?\'\\\/\+&%\$#\=~_\-@]*)*[^\.\,\?\"\'\(\)\[\]!;<>{}\s\x7F-\xFF])?)/;

//
// Format urls to include ti:systembrowser target in order to launch external browser
//
TiDeveloper.formatURIs = function(str)
{
	
	return $.gsub(str,TiDeveloper.URI_REGEX,function(m)
	{
		var x = m[0]
		
		if (!TiDeveloper.TLD.test(x) || x.indexOf('@') != -1)
		{
			return x;
		}
				
		if (x.indexOf('http://') == -1)
		{
			x = "http://" + x;
		}
		return '<a target="ti:systembrowser" href="' + x + '">' +x + '</a>';
	})
	
};

//
// Generic GSUB routing
//
$.extend(
{
	gsub:function(source,pattern,replacement)
	{
		if (typeof(replacement)=='string')
		{
			var r = String(replacement);
			replacement = function()
			{
				return r;
			}
		}
	 	var result = '', match;
	    while (source.length > 0) {
	      if (match = source.match(pattern)) {
			result += source.slice(0, match.index);
	        result += String(replacement(match));
	        source  = source.slice(match.index + match[0].length);
	      } else {
	        result += source, source = '';
	      }
	    }
		return result;
	}
});

TiDeveloper.make_url = function(base,params)
{
	var url = base;
	if (params)
	{
		url = url + '?';
		for (var p in params)
		{
			url+=encodeURIComponent(p)+'='+encodeURIComponent(String(params[p])) + "&"
		}
	}
	return url.substring(0,(url.length-1));
};

//
// load doc iframe - delay for perf reasons
//
// setTimeout(function()
// {
// 	var html = '<iframe src="http://titanium-js.appspot.com/" frameborder="0" height="80%" width="100%"></iframe>';
// 	$('#documentation').html(html);
// 	
// },800)
