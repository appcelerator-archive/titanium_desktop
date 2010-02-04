//
// application analytics module
//
(function()
{
	// How often to actually check for updates on the network in seconds.
	// 900 seconds == every 15 minutes.
	var UPDATE_CHECK_INTERVAL = 900;

	// How many milliseconds before the update check timer fires. The update
	// check timer will only launch an update check if the time passed since
	// the last update check is greater than UPDATE_CHECK_INTERVAL.
	var UPDATE_CHECK_TIMER_INTERVAL = 30000;

	var url = Titanium.App.getStreamURL("app-track");
	var guid = null;
	var sid = null;
	var debug = false;
	var initialized = false;
	var refresh_components = true;
	var update_check_timer = null;
	var analytics_spec_version = 2;

	// js returns minutes to add to local to get UTC,
	// Java returns ms to add to UTC to get local
	var tz_offset_mins = - (new Date().getTimezoneOffset());

	function send(qsv,async,timeout)
	{
		try
		{
			// If we're offline we don't even attempt these
			if (qsv.type != 'ti.start' && qsv.type != 'ti.end'
				&& Titanium.Network.online === false)
			{
				//TODO: we need to place these in DB and re-send later
				Titanium.API.debug("Not online -- skipping analytics");
				return;
			}

			// If we're offline we don't even attempt these
			if (!Titanium.App.analyticsEnabled)
			{
				Titanium.API.debug("Analytics disabled via tiapp.xml, skipping");
				return;
			}

			async = (typeof async=='undefined') ? true : async;
			qsv.mid = Titanium.Platform.id;
			qsv.guid = guid;
			qsv.sid = sid;
			qsv.mac_addr = Titanium.Platform.macaddress;
			qsv.osver = Titanium.Platform.version;
			qsv.platform = Titanium.platform;
			qsv.version = Titanium.version;
			qsv.app_version = Titanium.App.getVersion();
			qsv.os = Titanium.Platform.name;
			qsv.ostype = Titanium.Platform.ostype;
			qsv.osarch = Titanium.Platform.architecture;
			qsv.oscpu = Titanium.Platform.processorCount;
			qsv.un = Titanium.Platform.username;
			qsv.ip = Titanium.Platform.address;
			qsv.ver = analytics_spec_version;
			qsv.tz = tz_offset_mins;

			var qs = '';
			for (var p in qsv)
			{
				var v = typeof(qsv[p])=='undefined' ? '' : String(qsv[p]);
				qs+=p+'='+Titanium.Network.encodeURIComponent(v)+'&';
			}
			// this is asynchronous
			var xhr = Titanium.Network.createHTTPClient();
			if (timeout > 0)
			{
				xhr.setTimeout(timeout);
			}
			xhr.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
			if (debug)
			{
				xhr.onreadystatechange = function()
				{
					if (this.readyState==4)
					{
						Titanium.API.debug("++ received:"+this.responseText);
					}
				}
			}
			xhr.open('POST', url, async);
			xhr.send(qs);
		}
		catch(E)
		{
			Titanium.API.debug("Error sending analytics data: "+E);
		}
	}
	
	/** Undocumented, perhaps to be deprecated
	 * @no_tiapi(method=True,name=Analytics.addEvent,since=0.3)
	 * @no_tiapi Sends an analytics event associated with the application,
	 * @no_tiapi likely to be deprecated in favor of userEvent
	 * @no_tiarg(for=Analytics.addEvent,type=String,name=event) event name
	 * @no_tiarg(for=Analytics.addEvent,type=Object,name=data,optional=True) event data
	 */
	Titanium.API.set("Analytics.addEvent", function(event,data)
	{
		send({type:'app.addEvent',event:event,data:data});
	});

	/**
	 * @tiapi(method=True,name=Analytics.navEvent,since=0.7) Sends an analytics event associated with application navigation
	 * @tiarg(for=Analytics.navEvent,type=String,name=from) navigation starting point, the context we're leaving
	 * @tiarg(for=Analytics.navEvent,type=String,name=to) navigation ending point, the context to which we're going
	 * @tiarg(for=Analytics.navEvent,type=String,name=name,optional=True) event name
	 * @tiarg(for=Analytics.navEvent,type=Object,name=data,optional=True) event data
	 */
	Titanium.API.set("Analytics.navEvent", function(from,to,name,data)
	{
		if ((typeof(from)!='undefined') && (typeof(to)!='undefined'))
		{
			var payload = {from:from,to:to,data:data};
			payload = Titanium.JSON.stringify(payload);
			var event = ((typeof(name)!='undefined')?name:'app.nav');
			send({type:'app.nav',event:event,data:payload});
		}
	});
	
	/**
	 * @tiapi(method=True,name=Analytics.featureEvent,since=0.7) Sends an analytics event associated with application feature functionality
	 * @tiarg(for=Analytics.featureEvent,type=String,name=name) feature name
	 * @tiarg(for=Analytics.featureEvent,type=Object,name=data,optional=True) event data
	 */
	Titanium.API.set("Analytics.featureEvent", function(name,data)
	{
		if (typeof(name)!='undefined')
		{
			data = ((typeof(data)!='undefined') ? Titanium.JSON.stringify(data) : null);
			send({type:'app.feature',event:name,data:data});			
		}
	});

	/**
	 * @tiapi(method=True,name=Analytics.settingsEvent,since=0.7) Sends an analytics event associated with application settings or configuration
	 * @tiarg(for=Analytics.settingsEvent,type=String,name=name) settings name
	 * @tiarg(for=Analytics.settingsEvent,type=Object,name=data,optional=True) event data
	 */
	Titanium.API.set("Analytics.settingsEvent", function(name,data)
	{
		if (typeof(name)!='undefined')
		{
			data = ((typeof(data)!='undefined') ? Titanium.JSON.stringify(data) : null);
			send({type:'app.settings',event:name,data:data});
		}
	});
	
	/**
	 * @tiapi(method=True,name=Analytics.timedEvent,since=0.3) Sends an analytics event tracking the duration of an application action
	 * @tiarg(for=Analytics.timedEvent,type=String,name=event) event name
	 * @tiarg(for=Analytics.timedEvent,type=Date,name=start,optional=True) event start time (optional if duration is specified)
	 * @tiarg(for=Analytics.timedEvent,type=Date,name=stop,optional=True) event stop time (optional if duration is specified)
	 * @tiarg(for=Analytics.timedEvent,type=String,name=duration,optional=True) event duration in seconds (optional if both start and stop are specified)
	 * @tiarg(for=Analytics.timedEvent,type=Object,name=data,optional=True) event data
	 */
	Titanium.API.set("Analytics.timedEvent", function(name,start,stop,duration,data)
	{
		zeropad = function(maybe_small_number)
		{
			/* number in, two-digit (or more) string out */
			return ((maybe_small_number < 10) ? '0' : '') + maybe_small_number;
		}
		formatUTCstring = function(d)
		{
			/* format to yyyy-MM-dd'T'HH:mm:ss.SSSZ to be consistent with mobile's UTC timestamp strings */
			return d.getUTCFullYear().toString() + '-' +
			         zeropad(1 + d.getUTCMonth()) + '-' +
					 zeropad(d.getUTCDate()) + 'T' +
					 zeropad(d.getUTCHours()) + ':' +
					 zeropad(d.getUTCMinutes()) + ':' +
					 zeropad(d.getUTCSeconds()) + '+0000';
		}
		if (typeof(name)!='undefined')
		{
			payload = {};
			if (typeof(start)!='undefined')
			{
				payload.start = formatUTCstring(start);
			}
			if (typeof(stop)!='undefined')
			{
				payload.stop = formatUTCstring(stop);
			}
			if (typeof(duration)!='undefined')
			{
				payload.duration = duration;
			}
			if (typeof(data)!='undefined')
			{
				payload.data = data;
			}
			payload = Titanium.JSON.stringify(payload);
			send({type:'app.timed_event',event:name,data:payload});
		}
	});

	/**
	 * @tiapi(method=True,name=Analytics.userEvent,since=0.7) Sends an analytics event not covered by the other interfaces
	 * @tiarg(for=Analytics.userEvent,type=String,name=event) event name
	 * @tiarg(for=Analytics.userEvent,type=Object,name=data,optional=True) event data
	 */
	Titanium.API.set("Analytics.userEvent", function(name,data)
	{
		if (typeof(name)!='undefined')
		{
			data = ((typeof(data)!='undefined') ? Titanium.JSON.stringify(data) : null);
			send({type:'app.user',event:name,data:data});
		}
	});

	/**
	 * @tiapi(method=True,name=UpdateManager.startMonitor,since=0.4) Check the update service for a new version
	 * @tiarg(for=UpdateManager.startMonitor,name=component,type=String) Name of the component
	 * @tiarg(for=UpdateManager.startMonitor,name=callback,type=Function) Function callback to call when completed
	 * @tiarg(for=UpdateManager.startMonitor,name=interval,type=Number) Interval in milliseconds for how often to check for an update
	 * @tiresult(for=UpdateManager.startMonitor,type=Number) Returns a handle which should use used to cancel the monitor
	 */
	Titanium.API.set("UpdateManager.startMonitor", function(components,callback,interval)
	{
		if (interval == undefined || interval == null || interval < (60000) * 5)
		{
			interval = 60000*5;	//default is 5 minutes
		}
		
		function runCheck()
		{
			// perform the check
			for (var c=0;c<components.length;c++)
			{
				updateCheck(components[c],null,function(success,details)
				{
					if (success)
					{
						var list = Titanium.API.getInstalledComponents(refresh_components);
						var matches = [];
						for (var x=0;x<list.length;x++)
						{
							if (list[x].getName() == details.name)
							{
								matches.push(list[x]);
							}
						}
						if (matches.length > 0)
						{
							for (var x=0;x<matches.length;x++)
							{
								if (matches[x].getVersion()==details.version)
								{
									// if we found our version, nothing to do... we're OK
									return;
								}
							}
						}
						// update detected because you don't have it installed
						callback(details);
						
						// once we've refreshed we'll only refresh on updates
						refresh_components=false;
					}
				});	
			}
		};
		
		// schedule the timer to fire
		var timer = Titanium.setInterval(runCheck,interval);

		// go ahead and schedule
		Titanium.setTimeout(runCheck,1000);
		
		return timer;
	});

	/**
	 * @tiapi(method=True,name=UpdateManager.cancelMonitor,since=0.4) Check the update service for a new version
	 * @tiarg(for=UpdateManager.cancelMonitor,name=id,type=Number) The monitor id returned from startMonitor
	 */
	Titanium.API.set("UpdateManager.cancelMonitor", function(id)
	{
		Titanium.clearInterval(id);
	});

	/**
	 * @tiapi(property=True,name=UpdateManager.onupdate,since=0.4) Set the update handler implementation function that will be invoked when an update is detected
	 */
	Titanium.UpdateManager.onupdate = null;
		
	
	// NOTE: this is a private api and is not documented
	Titanium.API.set("UpdateManager.install", function(components,callback)
	{
		Titanium.API.installDependencies(components,function()
		{
			var components = Titanium.API.getInstalledComponents(true);
			if (callback)
			{
				callback(components);
			}
		});
	});

	/**
	 * @tiapi(method=True,name=UpdateManager.installAppUpdate,since=0.4)
	 * @tiapi Install an application update received from update monitor. This
	 * @tiapi method will cause the process to first be restarted for the update to begin.
	 * @tiarg[Object, updateSpec] Update spec object received from update service.
	 */
	Titanium.API.set("UpdateManager.installAppUpdate", function(updateSpec)
	{
		installAppUpdate(updateSpec);
	});

	Titanium.UpdateManager.compareVersions = function(newVersion, oldVersion)
	{
		// 1. Split on dots.
		// 2. For every dot do a comparison.
		// 3. If we get to the end of one of the arrays, the longer
		//    array is the larger version.
		var newVersionParts = newVersion.split(".");
		var oldVersionParts = oldVersion.split(".");

		while (newVersionParts.length > 0 && oldVersionParts.length > 0)
		{
			var newPart = newVersionParts[0];
			var oldPart = oldVersionParts[0];
			var result = false;

			// If both can't be parsed, compare them as strings.
			if (isNaN(newPart) && isNaN(oldPart))
			{
				if (newVersionParts[0] > oldVersionParts[0])
					return 1;
				else if (oldVersionParts[0] > newVersionParts[0])
					return -1;
			}
			else if (newPart != oldPart)
			{
				// Unparsable integer is always < than an integer
				if (isNaN(oldPart))
					return 1;
				else if (isNaN(newPart))
					return -1;
				else if (newPart > oldPart)
					return 1;
				else
					return -1;
			}

			newVersionParts.shift();
			oldVersionParts.shift();
		}

		// If the new version has a longer array, return true,
		// otherwise the old version is larger or equal.
		return newVersionParts.length - oldVersionParts.length;
	}

	function installAppUpdate(updateSpec)
	{
		// write our the new manifest for the update
		var datadir = Titanium.Filesystem.getApplicationDataDirectory();
		var update = Titanium.Filesystem.getFile(datadir,'.update');
		update.write(updateSpec.manifest);
		
		// restart ourselves to cause the install
		Titanium.App.restart();
	}
	

	function updateCheck(component,version,callback,limit)
	{
		try
		{
			if (!Titanium.Network.online)
			{
				return;
			}
			limit = (limit==undefined) ? 1 : limit;
			var url = Titanium.App.getStreamURL("release-list");
			var xhr = Titanium.Network.createHTTPClient();
			xhr.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
			var qs = 'version=' + Titanium.Network.encodeURIComponent(version) +
				'&name=' + Titanium.Network.encodeURIComponent(component) + 
				'&mid=' + Titanium.Network.encodeURIComponent(Titanium.Platform.id) +
				'&limit=' + limit +
				'&guid=' + Titanium.Network.encodeURIComponent(Titanium.App.getGUID()) +
				'&os=' + Titanium.platform +
				'&ostype=' + Titanium.Platform.ostype;
			xhr.onreadystatechange = function()
			{
				if (this.readyState==4)
				{
					try
					{
						var json = Titanium.JSON.parse(this.responseText);
						if (!json.success)
						{
							Titanium.API.error("Error response from update service: "+json.message);
							callback(false);
							return;
						}
						if (json.releases.length > 0)
						{
							// we might have an update
							// compare our version with the 
							// remote version
							var update = limit == 1 ? json.releases[0] : json.releases;
							callback(true,update);
						}
						else
						{
							callback(false);
						}
					}
					catch(e)
					{
						Titanium.API.error("Exception communicating to update service: "+e);
						callback(false);
					}
				}
			}
			xhr.open('POST', url, true);
			xhr.send(qs);
		}
		catch(e)
		{
			Titanium.API.error("Error performing update check = "+e);
			callback(false);
		}
	}
	
	function insertUpdateTimestamp(db,initial)
	{
		try
		{
			if (initial)
			{
				db.execute("insert into last_check values(strftime('%s','now'))")
			}
			else
			{
				db.execute("update last_check set time = strftime('%s','now')")
			}
		}
		catch(e)
		{
			Titanium.API.error("Error updating update db = "+e);
		}
	}
	function updateDetected(updateSpec)
	{
		// if we have a handler, delegate to that dude
		// and he's now responsible for doing the update stuff
		if (typeof Titanium.UpdateManager.onupdate == 'function')
		{
			Titanium.UpdateManager.onupdate(updateSpec);
			return;
		}
		
		var width = 450;
		var height = 170;
		var notes_url = null;
	
		if (updateSpec.release_notes)
		{
			width = 600;
			height = 350;
			// FOR NOW, turn off until we can clean up UI
			// notes_url = updateSpec.release_notes;
		}
	
		// ok, we'll handle it then...
		Titanium.UI.showDialog({
			'url': 'ti://tianalytics/update.html',
			'width': width,
			'height': height,
			'resizable':false,
			'parameters':{
				'name':Titanium.App.getName(),
				'icon':'file://'+Titanium.App.getIcon(),
				'ver_from':Titanium.App.getVersion(),
				'ver_to':updateSpec.version,
				'notes_url':notes_url 
			},
			'onclose':function(result)
			{
				if (result == 'install')
				{
					installAppUpdate(updateSpec);
				}
			}
		});
	}

	function checkForUpdate()
	{
		var db = null;
		var duration = null;

		try
		{
			db = Titanium.Database.open("app_updates");
			db.execute("create table if not exists last_check(time long)");

			// Seconds since the last update check or null if we've never done a check.
			var rs = db.execute("select strftime('%s','now')-time from last_check");
			var duration = rs.field(0);
			rs.close();
		}
		catch (e)
		{
			Titanium.API.error("Could not read UpdateManager last_check table: " + e);
			if (db)
				db.close();
			return;
		}

		try
		{
			// We aren't ready to do an update check yet.
			if (duration && duration < UPDATE_CHECK_INTERVAL)
				return;

			updateCheck('app-update', Titanium.App.getVersion(), function(success, update)
			{
				if (success && Titanium.UpdateManager.compareVersions(
					update.version,Titanium.App.getVersion()) > 0)
				{
					updateDetected(update);
				}
			});
		}
		catch(e)
		{
			Titanium.API.error("UpdateManager app update check failed: " + e);
			db.close();
			return;
		}

		// Record the last update.
		try
		{
			db.execute("DELETE FROM last_check"); // Delete old rows.
			db.execute("INSERT INTO last_check VALUES(strftime('%s','now'))");
		}
		catch (e)
		{
			Titanium.API.error("Could not update UpdateManager last_check table: " + e);
		}

		db.close();
	}
	
	Titanium.API.addEventListener(Titanium.APP_EXIT, function(event)
	{
		if (update_check_timer)
		{
			Titanium.clearTimeout(update_check_timer);
			update_check_timer=null;
		}
		if (initialized)
		{
			initialized = false;
			send({'event':'ti.end',type:'ti.end'},false,5000);
		}
	});

	Titanium.API.addEventListener(Titanium.PAGE_INITIALIZED, function(event)
	{
		try
		{
			if (initialized === true || !event.hasTitaniumObject)
			{
				return;
			}

			initialized = true;

			if (!Titanium.Platform.id)
			{
				Titanium.API.debug("No machine id found");
				return;
			}
			
			guid = Titanium.App.getGUID();
			sid = Titanium.Platform.createUUID();
			
			send({'event':'ti.start',type:'ti.start'});
			
			// schedule the update check
			update_check_timer = Titanium.setTimeout(function(){
				checkForUpdate();
			}, UPDATE_CHECK_TIMER_INTERVAL);
		}
		catch(e)
		{
			// never never never die in this function
			Titanium.API.error("Error: "+e);
		}
	});
})();
