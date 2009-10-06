(function()
{
	var initialized = false;
	var created = false;
	var db = null;

	Titanium.API.addEventListener(Titanium.EXIT, function(event)
	{
		if (initialized && db)
		{
			Titanium.API.debug("Shutting down tinetworkmodule.js");
			db.close();
			db = null;
		}
	});

	Titanium.API.addEventListener(Titanium.PAGE_INITIALIZED, function(event)
	{
		try
		{
			if (created || Titanium.Database == undefined || !event.hasTitaniumObject)
			{
				return;
			}

			created = true;
			
			// pull out host from URL
			function getHost(url)
			{
				var match = /http[s]{0,1}:\/\/(.*)/.exec(url);
				if (match && match.length == 2)
				{
					var idx = match[1].indexOf('/');
					if (idx == -1)
					{
						return match[1];
					}
					return match[1].substring(0,idx);
				}
				return null;
			}
			function initializeDB()
			{
				try
				{
					db = Titanium.Database.open("ti_network");
					db.execute("CREATE TABLE IF NOT EXISTS cookies(name TEXT, value TEXT, domain TEXT, expires INT)");
					db.execute("delete from cookies");
				}
				catch(E)
				{
					Titanium.API.error("Error initializing ti_network DB, Error: "+E);
				}
			}
			function setCookiesForHost(host,cookies)
			{
				if (!db) return;
				try
				{
					var tokens = cookies.split(';');
					var nv = tokens[0].split('=');
					var name = nv[0];
					var value = nv[1];
					var domain = host; //FIXME
					var expires = null; //FIXME
					//TODO: path
					Titanium.API.debug("setting cookie: "+name+" for "+host);
					db.execute("delete from cookies where name = ?", [name]);
					db.execute("insert into cookies values(?,?,?,?)",[name,value,domain,expires]);
				}
				catch(E)
				{
					Titanium.API.error("error setting cookie: "+cookies+", Error: "+E);
				}
			}
			function getCookiesForHost(host)
			{
				if (!db) return;
				try
				{
					var rs = db.execute("select name,value from cookies where domain = ?", host);
					var cookies = [];
					while (rs.isValidRow())
					{
						var name = rs.field(0);
						cookies.push(name+'='+rs.field(1));
						Titanium.API.debug("applying cookie: "+name+" for "+host);
						rs.next();
					}
					rs.close();
					return cookies.join('; ');
				}
				catch(E)
				{
					Titanium.API.error("error getting cookies for host: "+host+", Error: "+E);
				}
			}
			
			var originalHTTP = Titanium.Network.createHTTPClient;
			// patch our network client to support shared cookies
			Titanium.Network.createHTTPClient = function()
			{
				if (!initialized)
				{
					initialized=true;
					initializeDB();
				}
				var xhr = originalHTTP.call(this);
				var originalOpener = xhr.open;
				var hostname = null;
				
				xhr.open = function(method,url)
				{
					if (!Titanium.Network.online)
					{
						throw "not currently online";
					}
					// override to handle set/get of session cookies
					hostname = getHost(url);
					if (hostname)
					{
						var cookies = getCookiesForHost(hostname);
						if (cookies)
						{
							xhr.setRequestHeader('Cookie',cookies);
						}
					}
					xhr._onreadystatechange = xhr.onreadystatechange;
					xhr.onreadystatechange = function()
					{
						if (this.readyState == this.HEADERS_RECEIVED)
						{
							var c = xhr.getResponseHeader('Set-Cookie');
							if (c)
							{
								setCookiesForHost(hostname,c);
							}
						}
						try
						{
							if (xhr._onreadystatechange)
							{
								xhr._onreadystatechange.call(this);
							}
							if (this.readyState == 4)
							{
								xhr._onreadystatechange = null;
								originalOpener = null;
								xhr.onreadystatechange = null;
								Titanium.API.debug("DONE !");
								xhr = null;
							}
						}
						catch (EX)
						{
							Titanium.API.error("error dispatching ready state listener = "+EX);
						}
					};
					
					return originalOpener.apply(this,arguments);
				};
				
				return xhr;
			};
		}
		catch(E)
		{
			Titanium.API.error("Error in tinetworkmodule.js => "+E);
		}
	});
			
})();
	
