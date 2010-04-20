(function()
{
	Titanium.Analytics.sendEvent = function(data)
	{
		// If we're offline we don't even attempt to send Analytics.
		//TODO: we need to place these in DB and re-send later
		if (Titanium.Network.online === false)
		{
			Titanium.API.debug("Not online -- skipping analytics");
			return;
		}
		if (!Titanium.App.analyticsEnabled)
		{
			Titanium.API.debug("Analytics disabled via tiapp.xml, skipping");
			return;
		}

		queryString = "";
		for (var key in data)
		{
			queryString += key + "=" + (data[key] === undefined ?
				 '' : Titanium.Network.encodeURIComponent(data[key])) + '&';
		}

		// Send the event natively and asynchronously.
		Titanium.Analytics._sendEvent(queryString);
	};
	
	/** Undocumented, perhaps to be deprecated
	 * @no_tiapi(method=True,name=Analytics.addEvent,since=0.3)
	 * @no_tiapi Send an analytics event associated with the application,
	 * @no_tiapi likely to be deprecated in favor of userEvent
	 * @no_tiarg[String, event] event name
	 * @no_tiarg[String. data] event data
	 */
	Titanium.Analytics.addEvent = function(event,data)
	{
		Titanium.Analytics.sendEvent({type:'app.addEvent',event:event,data:data});
	};

	/**
	 * @tiapi(method=True,name=Analytics.navEvent,since=0.7)
	 * @tiapi Send an analytics event associated with application navigation
	 * @tiarg[String, from] Navigation starting point, the context we're leaving.
	 * @tiarg[String, to] Navigation ending point, the context to which we're going.
	 * @tiarg[String, name] Event name.
	 * @tiarg[Object, data] Extra event data to pass along. This will be converted to JSON.
	 */
	Titanium.Analytics.navEvent = function(from, to, name, data)
	{
		if (from === undefined || to === undefined)
			return;

		Titanium.Analytics.sendEvent({
			type: 'app.nav', 
			event: name === undefined ? "app.nav" : name,
			data: JSON.stringify({from:from, to:to, data:data})
		});
	};

	/**
	 * @tiapi(method=True,name=Analytics.featureEvent,since=0.7)
	 * @tiapi Send an analytics event associated with application feature functionality
	 * @tiarg[String, name] Feature name.
	 * @tiarg[Object, data] Extra event data to pass along. This will be converted to JSON.
	 */
	Titanium.Analytics.featureEvent = function(name, data)
	{
		if (name === undefined)
			return;

		Titanium.Analytics.sendEvent({
			type: 'app.feature',
			event: name,
			data: data === undefined ? null : JSON.stringify(data)
		});
	};

	/**
	 * @tiapi(method=True,name=Analytics.settingsEvent,since=0.7)
	 * @tiapi Send an analytics event associated with application settings or configuration
	 * @tiarg[String, name] Setting name.
	 * @tiarg[Object, data] Extra event data to pass along. This will be converted to JSON.
	 */
	Titanium.Analytics.settingsEvent = function(name,data)
	{
		if (name === undefined)
			return;

		Titanium.Analytics.sendEvent({
			type: 'app.settings',
			event: name,
			data: data === undefined ? null : JSON.stringify(data)
		});
	};
	
	/**
	 * @tiapi(method=True,name=Analytics.timedEvent,since=0.3)
	 * @tiapi Send an analytics event tracking the duration of an application action
	 * @tiarg[String, event] Event name
	 * @tiarg[Date, start] Event start time (optional if duration is specified).
	 * @tiarg[Date, stop] Event stop time (optional if duration is specified).
	 * @tiarg[Number, duration] Duration in seconds (optional if both start and stop
	 * @tiarg are specified)
	 * @tiarg[Object, data] Extra event data to pass along. This will be converted to JSON.
	 */
	Titanium.Analytics.timedEvent = function(name,start,stop,duration,data)
	{
		// Number in, two-digit (or more) string out
		var zeropad = function(maybe_small_number)
		{
			return ((maybe_small_number < 10) ? '0' : '') + maybe_small_number;
		}

		// format to yyyy-MM-dd'T'HH:mm:ss.SSSZ to be consistent with
 		// Titanium Mobile UTC timestamp strings.
		var formatUTCstring = function(d)
		{
			return d.getUTCFullYear().toString() + '-' +
				zeropad(1 + d.getUTCMonth()) + '-' +
				 zeropad(d.getUTCDate()) + 'T' +
				 zeropad(d.getUTCHours()) + ':' +
				 zeropad(d.getUTCMinutes()) + ':' +
				 zeropad(d.getUTCSeconds()) + '+0000';
		}

		if (name === undefined)
			return;

		payload = {};
		if (start !== undefined)
			payload.start = formatUTCstring(start);
		if (stop !== undefined)
			payload.stop = formatUTCstring(stop);
		if (duration !== undefined)
			payload.duration = duration;
		if (data !== undefined)
			payload.data = data;
		Titanium.Analytics.sendEvent({
			type: 'app.timed_event',
			event: name,
			data: JSON.stringify(payload)
		});
	};

	/**
	 * @tiapi(method=True,name=Analytics.userEvent,since=0.7)
	 * @tiapi Send an analytics event not covered by the other interfaces
	 * @tiarg[String, name] Event name.
	 * @tiarg[Object, data] Extra event data to pass along. This will be converted to JSON.
	 */
	Titanium.Analytics.userEvent = function(name,data)
	{
		if (name === undefined)
			return;

		Titanium.Analytics.sendEvent({
			type: 'app.user',
			event: name,
			data: data === undefined ? null : JSON.stringify(data)
		});
	};

	Titanium.UpdateManager = {};

	/**
	 * @tiapi(method=True,name=UpdateManager.startMonitor,since=0.4) Check the update service for a new version
	 * @tiarg(for=UpdateManager.startMonitor,name=component,type=String) Name of the component
	 * @tiarg(for=UpdateManager.startMonitor,name=callback,type=Function) Function callback to call when completed
	 * @tiarg(for=UpdateManager.startMonitor,name=interval,type=Number) Interval in milliseconds for how often to check for an update
	 * @tiresult(for=UpdateManager.startMonitor,type=Number) Returns a handle which should use used to cancel the monitor
	 */
	Titanium.UpdateManager.startMonitor = function(components,callback,interval)
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
						var list = Titanium.API.getInstalledComponents(refreshComponents);
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
						refreshComponents = false;
					}
				});	
			}
		};
		
		// schedule the timer to fire
		var timer = Titanium.setInterval(runCheck,interval);

		// go ahead and schedule
		Titanium.setTimeout(runCheck,1000);
		
		return timer;
	};

	/**
	 * @tiapi(method=True,name=UpdateManager.cancelMonitor,since=0.4) Check the update service for a new version
	 * @tiarg(for=UpdateManager.cancelMonitor,name=id,type=Number) The monitor id returned from startMonitor
	 */
	Titanium.UpdateManager.cancelMonitor = function(id)
	{
		Titanium.clearInterval(id);
	};

	/**
	 * @tiapi(property=True,name=UpdateManager.onupdate,since=0.4) Set the update handler implementation function that will be invoked when an update is detected
	 */
	Titanium.UpdateManager.onupdate = null;
		
	
	// NOTE: this is a private api and is not documented
	Titanium.UpdateManager.install = function(components,callback)
	{
		Titanium.API.installDependencies(components,function()
		{
			var components = Titanium.API.getInstalledComponents(true);
			if (callback)
			{
				callback(components);
			}
		});
	};

	/**
	 * @tiapi(method=True,name=UpdateManager.installAppUpdate,since=0.4)
	 * @tiapi Install an application update received from update monitor. This
	 * @tiapi method will cause the process to first be restarted for the update to begin.
	 * @tiarg[Object, updateSpec] Update spec object received from update service.
	 */
	Titanium.UpdateManager.installAppUpdate = function(updateSpec)
	{
		installAppUpdate(updateSpec);
	};

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
			'url': 'ti://tinetwork/update.html',
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
		if (updateCheckTimer)
		{
			Titanium.clearTimeout(updateCheckTimer);
			updateCheckTimer = null;
		}

		Titanium.Analytics.sendEvent({'event': 'ti.end', type: 'ti.end'});
	});

	Titanium.Analytics.sendEvent({'event': 'ti.start', 'type': 'ti.start'});

	if (Titanium.App.updateMonitorEnabled)
	{
		// How often to actually check for updates on the network in seconds.
		// 900 seconds == every 15 minutes.
		var UPDATE_CHECK_INTERVAL = 900;

		// How many milliseconds before the update check timer fires. The update
		// check timer will only launch an update check if the time passed since
		// the last update check is greater than UPDATE_CHECK_INTERVAL.
		var UPDATE_CHECK_TIMER_INTERVAL = 30000;

		var refreshComponents = true;
		var updateCheckTimer = updateCheckTimer = 
			Titanium.setTimeout(function() { checkForUpdate(); }, UPDATE_CHECK_TIMER_INTERVAL);
	}
})();
