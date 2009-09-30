//
// application analytics module
//
(function()
{
	var update_check_delay = 30000; // how many ms before we initiate check
	var update_check_interval_secs = (60000 * 15) / 1000; // once per 15 min
	
	var url = Titanium.App.getStreamURL("app-track");
	var guid = null;
	var sid = null;
	var debug = false;
	var initialized = false;
	var window = null;
	var refresh_components = true;
	var update_check_timer = null;
	
	function send(qsv,async,timeout)
	{
		try
		{
			// if we're offline we don't even attempt these
			if (qsv.event!='ti.start' && qsv.event!='ti.end' && Titanium.Network.online===false)
			{
				//TODO: we need to place these in DB and re-send later
				Titanium.API.debug("we're not online - skipping analytics");
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
	
	/**
	 * @tiapi(method=True,name=Analytics.addEvent,since=0.3) Sends an analytics event associated with the application
	 * @tiarg(for=Analytics.addEvent,type=String,name=event) event name
	 * @tiarg(for=Analytics.addEvent,type=Object,name=data,optional=True) event data
	 */
	Titanium.API.set("Analytics.addEvent", function(event,data)
	{
		send({'event':event,'data':data});
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
		var timer = window.setInterval(runCheck,interval);

		// go ahead and schedule
		window.setTimeout(runCheck,1000);
		
		return timer;
	});

	/**
	 * @tiapi(method=True,name=UpdateManager.cancelMonitor,since=0.4) Check the update service for a new version
	 * @tiarg(for=UpdateManager.cancelMonitor,name=id,type=Number) The monitor id returned from startMonitor
	 */
	Titanium.API.set("UpdateManager.cancelMonitor", function(id)
	{
		window.clearInterval(id);
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
	 * @tiapi(method=True,name=UpdateManager.installAppUpdate,since=0.4) Install an application update received from update monitor. This method will cause the process to first be restarted for the update to begin.
	 * @tiarg(for=UpdateManager.installAppUpdate,name=spec,type=Object) Update spec object received from update service.
	 */
	Titanium.API.set("UpdateManager.installAppUpdate", function(updateSpec)
	{
		installAppUpdate(updateSpec);
	});
	
	
	function installAppUpdate(updateSpec)
	{
		// write our the new manifest for the update
		var datadir = Titanium.Filesystem.getApplicationDataDirectory();
		var update = Titanium.Filesystem.getFile(datadir,'.update');
		update.write(updateSpec.manifest);
		
		// restart ourselves to cause the install
		Titanium.Process.restart();
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
			var qs = 'version='+Titanium.Network.encodeURIComponent(version)+'&name='+Titanium.Network.encodeURIComponent(component)+'&mid='+Titanium.Network.encodeURIComponent(Titanium.Platform.id)+'&limit='+limit+'&guid='+Titanium.Network.encodeURIComponent(Titanium.App.getGUID());
			xhr.onreadystatechange = function()
			{
				if (this.readyState==4)
				{
					try
					{
						var json = window.Titanium.JSON.parse(this.responseText);
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
			xhr.open('POST',url,true);
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
		window.Titanium.UI.showDialog({
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
	function isUpdateRequired(newVersion, oldVersion)
	{
		// for now, trivial check
		// in the future we may want something more sophisticated
		return oldVersion!=newVersion;
	}
	function sendUpdateCheck()
	{
		updateCheck('app-update',Titanium.App.getVersion(),function(success,update)
		{
			if (success && isUpdateRequired(update.version,Titanium.App.getVersion()))
			{
				updateDetected(update);
			}
		});
	}
	function checkForUpdate()
	{
		var db = Titanium.Database.open("app_updates");
		var initial = false;
		db.execute("create table if not exists last_check(time long)");
		try
		{
			var rs = db.execute("select strftime('%s','now')-time from last_check");
			var duration = rs.field(0);
			rs.close();
			if (duration == null)
			{
				initial = true;
			}
			if (!duration || duration >= update_check_interval_secs)
			{
				// time to perform check
				sendUpdateCheck();
			}
		}
		catch(e)
		{
			Titanium.API.error("Error in ti.Analytics checkForUpdate. Error="+e);	
		}
		insertUpdateTimestamp(db,initial);
		db.close();
	}
	
	Titanium.API.addEventListener(Titanium.EXIT, function(event)
	{
		if (update_check_timer)
		{
			window.clearTimeout(update_check_timer);
			update_check_timer=null;
		}
		if (initialized)
		{
			window = null;
			initialized = false;
			send({'event':'ti.end'},false,5000);
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
			
			// map in our window scope
			window = event.scope;
			
			guid = Titanium.App.getGUID();
			sid = Titanium.Platform.createUUID();
			
			send({'event':'ti.start'});
			
			// schedule the update check
			update_check_timer = window.setTimeout(function(){
				checkForUpdate();
			},update_check_delay);
		}
		catch(e)
		{
			// never never never die in this function
			Titanium.API.error("Error: "+e);
		}
	});
})();
