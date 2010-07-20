// TODO - add tests for Titanium.API methods and properties that are called else where in other tests.
// this test suite contains all of the missing API coverage as of 4-July-20009

describe("ti.API tests",
{
	// test the logging functions
	test_logging_methods: function()
	{
		value_of(Titanium.API.critical).should_be_function();
		value_of(Titanium.API.debug).should_be_function();
		value_of(Titanium.API.error).should_be_function();
		value_of(Titanium.API.fatal).should_be_function();
		value_of(Titanium.API.notice).should_be_function();
		value_of(Titanium.API.trace).should_be_function();
		value_of(Titanium.API.warn).should_be_function();
		
		//new 1.0
		value_of(Titanium.API.setLogLevel).should_be_function();

		Titanium.API.critical("this is a critical message");
		Titanium.API.debug("this is a debug message");
		Titanium.API.error("this is a error message");
		Titanium.API.fatal("this is a fatal message");

		Titanium.API.notice("this is a notice message");
		Titanium.API.trace("this is a trace message");
		Titanium.API.warn("this is a warn message");
		
		Titanium.API.print("this is a print message to stdout");
	},
	// test the logging functions
	test_log_method: function()
	{
		value_of(Titanium.API.log).should_be_function();

		Titanium.API.log(Titanium.API.CRITICAL,"this is a log message with severity Titanium.API.CRITICAL");
		Titanium.API.log(Titanium.API.DEBUG,"this is a log message with severity Titanium.API.DEBUG");
		Titanium.API.log(Titanium.API.ERROR,"this is a log message with severity Titanium.API.ERROR");
		Titanium.API.log(Titanium.API.FATAL,"this is a log message with severity Titanium.API.FATAL");
		Titanium.API.log(Titanium.API.INFO,"this is a log message with severity Titanium.API.INFO");
		Titanium.API.log(Titanium.API.NOTICE,"this is a log message with severity Titanium.API.NOTICE");
		Titanium.API.log(Titanium.API.TRACE,"this is a log message with severity Titanium.API.TRACE");
		Titanium.API.log(Titanium.API.WARN,"this is a log message with severity Titanium.API.WARN");

	},

	// test the logging functions
	test_setLogLevel_method: function()
	{
		//new 1.0
		value_of(Titanium.API.setLogLevel).should_be_function();
		value_of(Titanium.API.log).should_be_function();

		Titanium.API.setLogLevel(Titanium.API.WARN);
		Titanium.API.log(Titanium.API.FATAL,"this is a log message with severity Titanium.API.FATAL should be logged");
		Titanium.API.log(Titanium.API.CRITICAL,"this is a log message with severity Titanium.API.CRITICAL should be logged");
		Titanium.API.log(Titanium.API.ERROR,"this is a log message with severity Titanium.API.ERROR should be logged");
		Titanium.API.log(Titanium.API.WARN,"Logging severity set to Titanium.API.WARN");
		Titanium.API.log(Titanium.API.NOTICE,"this is a log message with severity Titanium.API.NOTICE should not be logged");
		Titanium.API.log(Titanium.API.INFO,"this is a log message with severity Titanium.API.INFO should not be logged");
		Titanium.API.log(Titanium.API.DEBUG,"this is a log message with severity Titanium.API.DEBUG should not be logged");
		Titanium.API.log(Titanium.API.TRACE,"this is a log message with severity Titanium.API.TRACE should not be logged");

		Titanium.API.setLogLevel(Titanium.API.FATAL);
	},

	validate_properties: function()
	{
		value_of(Titanium.API.APP_UPDATE).should_not_be_null();
		value_of(Titanium.API.CRITICAL).should_not_be_null();
		value_of(Titanium.API.DEBUG).should_not_be_null();
		value_of(Titanium.API.EQ).should_not_be_null();
		value_of(Titanium.API.ERROR).should_not_be_null();
		value_of(Titanium.API.FATAL).should_not_be_null();
		value_of(Titanium.API.GT).should_not_be_null();
		value_of(Titanium.API.GTE).should_not_be_null();
		value_of(Titanium.API.INFO).should_not_be_null();
		value_of(Titanium.API.LT).should_not_be_null();
		value_of(Titanium.API.LTE).should_not_be_null();
		value_of(Titanium.API.MOBILESDK).should_not_be_null();
		value_of(Titanium.API.NOTICE).should_not_be_null();
		value_of(Titanium.API.RUNTIME).should_not_be_null();
		value_of(Titanium.API.SDK).should_not_be_null();
		value_of(Titanium.API.TRACE).should_not_be_null();
		value_of(Titanium.API.UNKNOWN).should_not_be_null();
		value_of(Titanium.API.WARN).should_not_be_null();

		value_of(Titanium.API.createDependency).should_be_function();
		value_of(Titanium.API.critical).should_be_function();
		value_of(Titanium.API.debug).should_be_function();
		value_of(Titanium.API.error).should_be_function();
		value_of(Titanium.API.fatal).should_be_function();
		value_of(Titanium.API.fireEvent).should_be_function();
		value_of(Titanium.API.get).should_be_function();
		value_of(Titanium.API.getApplication).should_be_function();			 //done
		value_of(Titanium.API.getComponentSearchPaths).should_be_function();
		value_of(Titanium.API.getInstalledComponents).should_be_function();
		value_of(Titanium.API.getInstalledMobileSDKs).should_be_function();
		value_of(Titanium.API.getInstalledModules).should_be_function();
		value_of(Titanium.API.getInstalledRuntimes).should_be_function();
		value_of(Titanium.API.getInstalledSDKs).should_be_function();
		value_of(Titanium.API.info).should_be_function();
		value_of(Titanium.API.installDependencies).should_be_function();
		value_of(Titanium.API.log).should_be_function();
		value_of(Titanium.API.notice).should_be_function();
		value_of(Titanium.API.readApplicationManifest).should_be_function();
		value_of(Titanium.API.addEventListener).should_be_function();
		value_of(Titanium.API.set).should_be_function();
		value_of(Titanium.API.trace).should_be_function();
		value_of(Titanium.API.removeEventListener).should_be_function();
		value_of(Titanium.API.warn).should_be_function();
		
		//new 1.0
		value_of(Titanium.API.setLogLevel).should_be_function();
		value_of(Titanium.API.print).should_be_function();
	},
	// test the application components API
	test_components: function()
	{
		Titanium.API.error("Titanium.API.getInstalledComponents()");
		// now that the app is finished testing, test the Titanium.API runtime calls.
		// this should return a valid list of runtime components installed with this app.		
		var apiInstalledComponents1 = Titanium.API.getInstalledComponents();
		
		value_of(apiInstalledComponents1).should_be_object();
		if ( apiInstalledComponents1.length )
		{
			Titanium.API.error("Titanium.API.getInstalledComponents() contains:");
			for (i=0; i<apiInstalledComponents1.length; i++)
			{
				var item = apiInstalledComponents1[i];
				value_of(item).should_be_object();
				Titanium.API.info("apiInstalledComponents["+i+"]= name: '" + item.getName() + 
								  "' type: '"+item.getType()+
								  "' version: '"+item.getVersion()+
								  "' path '" + item.getPath() + "'");
			}
		}
		else 
		{
			Titanium.API.error("Titanium.API.getInstalledComponents() returned an empty list");
		}		
	},
	// test the application dependancies API
	test_dependancies: function()
	{
		// create a dependancy  what does this mean?  what are we trying to accomplish?
		var dep = Titanium.API.createDependency(Titanium.API.RUNTIME, "test", "0.0.1", Titanium.API.EQ);
		
		value_of(dep).should_be_object();
		value_of(dep.getName).should_be_function();
		value_of(dep.getType).should_be_function();
		value_of(dep.getVersion).should_be_function();
		
		Titanium.API.info("processing dependancy '" + dep.getName() + 
						  "' type: '"+dep.getType()+
						  "' version: '"+dep.getVersion()+"'");
	},
	// test the installed runtime component functions
	test_installed_runtime: function()
	{
		Titanium.API.error("Titanium.API.getInstalledRuntimes()");
		// now that the app is finished testing, test the Titanium.API runtime calls.
		// this should return a valid list of runtime components installed with this app.		
		var apiInstalledRuntimes1 = Titanium.API.getInstalledRuntimes();
		
		value_of(apiInstalledRuntimes1).should_be_object();
		if ( apiInstalledRuntimes1.length )
		{
			Titanium.API.error("Titanium.API.getInstalledRuntimes() contains:");
			for (i=0; i<apiInstalledRuntimes1.length; i++)
			{
				var item = apiInstalledRuntimes1[i];
				value_of(item).should_be_object();
			
				Titanium.API.info("apiInstalledRuntimes["+i+"] name: '" + item.getName() + 
								  "' type: '"+item.getType()+
								  "' version: '"+item.getVersion()+
								  "' path '" + item.getPath() + "'");
			}
		}
		else 
		{
			Titanium.API.error("Titanium.API.getInstalledRuntimes() returned an empty list");
		}
	},
	// test the installed modules component functions
	test_installed_modules: function()
	{
		Titanium.API.error("Titanium.API.getInstalledModules()");
		// now that the app is finished testing, test the Titanium.API runtime calls.
		// this should return a valid list of runtime components installed with this app.		
		var apiInstalledModules1 = Titanium.API.getInstalledModules();
		
		value_of(apiInstalledModules1).should_be_object();
		if ( apiInstalledModules1.length )
		{
			Titanium.API.error("Titanium.API.getInstalledModules() contains:");
			for (i=0; i<apiInstalledModules1.length; i++)
			{
				var item = apiInstalledModules1[i];
				value_of(item).should_be_object();
				
				Titanium.API.info("apiInstalledModules["+i+"] name: '" + item.getName() + 
								  "' type: '"+item.getType()+
								  "' version: '"+item.getVersion()+
								  "' path '" + item.getPath() + "'");
			}
		}
		else 
		{
			Titanium.API.error("Titanium.API.getInstalledModules() returned an empty list");
		}
	},

	// test the API SDK functions
	test_api_sdk: function()
	{
		Titanium.API.error("Titanium.API.getInstalledSDKs()");
		// now that the app is finished testing, test the Titanium.API runtime calls.
		// this should return a valid list of runtime components installed with this app.		
		var sdks = Titanium.API.getInstalledSDKs();
		value_of(sdks).should_be_array();
		if ( sdks.length )
		{
			Titanium.API.error("Titanium.API.getInstalledSDKs() contains:");
			for (i=0; i<sdks.length; i++)
			{
				var item = sdks[i];
				value_of(item).should_be_object();
				
				Titanium.API.info("apiInstalledSDKs["+i+"] name: '" + item.getName() + 
								  "' type: '"+item.getType()+
								  "' version: '"+item.getVersion()+
								  "' path '" + item.getPath() + "'");
			}
		}
		else 
		{
			Titanium.API.error("Titanium.API.getInstalledSDKs() returned an empty list");
		}

		// test the objects in API		
		var app = Titanium.API.getApplication();
	  
		value_of(app).should_not_be_null();
		if ( app ) 
		{
			value_of(app).should_be_object();

			// every module, runtime object and SDK should have a component entry		
			var components = app.getComponents();
			value_of(components).should_be_array();
			
			// once we have the list of components from the application 
			// object, we need to  verify that the lists are the same 
			// that we get through the application object.

			Titanium.API.info("checking installed SDKs[] ");
			var bFound = false;
			for (j=0; j<sdks.length; j++ )
			{
				var module = sdks[j];
				Titanium.API.info("processing SDK component '" + module.getName() + 
								  "' type: '"+module.getType()+
								  "' version: '"+module.getVersion()+
								  "' path '" + module.getPath() + "'");
				
				
				for ( i=0; i<components.length; i++ )
				{
					var am = components[i];
					value_of(am).should_not_be_null();
					
					Titanium.API.info("processing component '" + am.getName() + 
									  "' type: '"+am.getType()+
									  "' version: '"+am.getVersion()+
									  "' path '" +am.getPath() + "'");

					if ( module.getName() == am.getName() &&
						 module.getType() == am.getType() &&
						 module.getPath() == am.getPath())
					{
						Titanium.API.trace("match SDK  "+module.getName() +" to component entry");
						bFound = true;
						break;
					}
				}
			}
			
			if ( !bFound )
			{
				// fail the test if we don't find a value.						
				Titanium.API.fatal("failed to match SDK object to list of loaded components");
				// this is not a real bug.  this is only meant for developer environments.
			}
		}
	},
	
	test_api_installed_mobile_sdks: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();

		// every module, runtime object and SDK should have a component entry		
		var components = app.getComponents();
		value_of(components).should_be_array();
		
		// once we have the list of components from the application 
		// object, we need to  verify that the lists are the same 
		// that we get through the application object.
		var mobileSDKs = Titanium.API.getInstalledMobileSDKs();
		value_of(mobileSDKs).should_be_array();

		var bFound = false;
		for (j=0; j<mobileSDKs.length; j++ )
		{
			var module = mobileSDKs[j];
			Titanium.API.info("processing mobile SDK module '" + module.getName() + 
							  "' type: '"+module.getType()+
							  "' version: '"+module.getVersion()+
							  "' path '" + module.getPath() + "'");
			
			for ( i=0; i<components.length; i++ )
			{
				if ( module.getName() == components[i].getName() &&
					 module.getType() == components[i].getType() &&
					 module.getPath() == components[i].getPath())
				{
					Titanium.API.trace("match avialable mobile SDK  "+module.getName() +" to components entry");
					bFound = true;
					break;
				}
			}
			
			if ( !bFound )
			{
				// don't fail the test if we don't find a value.  this is really just for titanium development.
				Titanium.API.warn("failed to match mobile SDK object to list of loaded components");
				// this is not a real bug.  this is only meant for developer environments.
			}
		}
	},
	// test the api arguments.
	test_api_arguments: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
	  
		value_of(app).should_not_be_null();
		if ( app ) 
		{
			value_of(app).should_be_object();
			var argv = app.getArguments();

			if ( argv )
			{
				value_of(argv).should_be_object();
				value_of(argv.length).should_not_be(0);
				
				// print out all the arguments to drillbit.
				for (i=0; i<argv.length; i++)
				{
					Titanium.API.info("argv["+i+"] = "+argv[i]);
				}
				
				var bHasit = app.hasArgument("results");
				value_of(bHasit).should_be_true();
				
				var results = app.getArgumentValue("results");
				value_of(results).should_not_be_null();
			}
		}
	},
	// test the module component functions
	test_api_module: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
	  
		value_of(app).should_not_be_null();
		if ( app ) 
		{
			value_of(app).should_be_object();
			
			var pid = app.getPID();
			
			// isCurrent is true when we are the currently running app
			if (app.isCurrent())
			{
				// the PID should not be null when we are the current app
				value_of(pid).should_not_be_null();
				Titanium.API.info("Application PID = "+pid);
			}
			else
			{
				// the pid should be null when we are not the current running app.				
				// I don't think we should ever get here in the unit test since 
				// we should always be the current app.
				value_of(pid).should_be_null();
			}
		}
	},
	// test the application path functions
	test_api_component_Search_Paths: function()
	{
	    var componentSearchPaths = Titanium.API.getComponentSearchPaths();
	    
	    value_of(componentSearchPaths).should_be_object();
	    value_of(componentSearchPaths.length).should_not_be(0);
	    
	    
        Titanium.API.info("dump component search paths")
	    for (i=0; i<componentSearchPaths.length; i++)
	    {
	        Titanium.API.info(componentSearchPaths[i]);
	    }
	},
	
	test_api_events_as_async: function(callback)
	{
		// create an event
		var w = Titanium.API.addEventListener("foo", function()
		{
			callback.passed();
		});
		// make sure we have a callback for it
		value_of(w).should_be_function();

		// fire it off
		Titanium.API.fireEvent("foo");

		// unregister the event when we are done.
		Titanium.API.removeEventListener("foo", w);

	},
	
	test_api_global_object: function()
	{
		// set a global object
		Titanium.API.set("foo", "bar" );
		
		value_of(Titanium.API.foo).should_be_string();
		value_of(Titanium.API.foo).should_be("bar");
		
		var str = Titanium.API.get("foo");
		
		value_of(str).should_be_string();
		value_of(str).should_be("bar");
	},

	test_api_run_on_main: function()
	{
		function test(value) {
			Titanium.API.foo = value;
		}

		Titanium.API.runOnMainThread(test, "works!");
		value_of(Titanium.API.foo).should_be("works!");
	}
});
