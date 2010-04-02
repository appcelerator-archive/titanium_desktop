describe("ti.API.Application tests",
{
	validate_properties: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		
		value_of(app).should_not_be_null();
		if ( app ) 
		{
			value_of(app).should_be_object();
			value_of(app.getArgumentValue).should_be_function();
			value_of(app.getArguments).should_be_function();
			value_of(app.getAvailableComponents).should_be_function();
			value_of(app.getAvailableModules).should_be_function();
			value_of(app.getAvailableRuntimes).should_be_function();
			value_of(app.getBundledComponents).should_be_function();
			value_of(app.getBundledModules).should_be_function();
			value_of(app.getBundledRuntimes).should_be_function();
			value_of(app.getComponents).should_be_function();
			value_of(app.getDataPath).should_be_function();
			value_of(app.getDependencies).should_be_function();
			value_of(app.getExecutablePath).should_be_function();
			value_of(app.getGUID).should_be_function();
			value_of(app.getID).should_be_function();
			value_of(app.getManifest).should_be_function();
			value_of(app.getManifestPath).should_be_function();
			value_of(app.getModules).should_be_function();
			value_of(app.getName).should_be_function();
			value_of(app.getPID).should_be_function();
			value_of(app.getPath).should_be_function();
			value_of(app.getResourcesPath).should_be_function();
			value_of(app.getRuntime).should_be_function();
			value_of(app.getVersion).should_be_function();
			value_of(app.hasArgument).should_be_function();
			value_of(app.isCurrent).should_be_function();
			value_of(app.resolveDependencies).should_be_function();
		}		
	},

	test_loaded_components: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();
		
		// get the components loaded by the application
		var loadedComponents = app.getComponents();
		value_of(loadedComponents).should_not_be_null();

		// validate the loaded components and verify 
		// that these components are supposed to be here.
		// these components should be part of the bundle or installed or both.
		for ( i =0; i < loadedComponents.length; i++ )
		{
			var  item = loadedComponents[i];
			// do we have a component?
			value_of(item).should_not_be_null();
			value_of(item).should_be_object();
			value_of(item.getName).should_be_function();
			value_of(item.getPath).should_be_function();
			value_of(item.getType).should_be_function();
			value_of(item.getVersion).should_be_function();
			value_of(item.isBundled).should_be_function();
			value_of(item.isLoaded).should_be_function();
			
			// loaded components should always indicate so...
			value_of(item.isLoaded()).should_be_true();

			Titanium.API.info("processing loaded component '" + item.getName() + 
							  "' type: '"+item.getType()+
							  "' version: '"+item.getVersion()+
							  "' path '" + item.getPath() + "'");
		}
	},
	
	test_installed_components: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();
		
		// get the components loaded by the application
		var installedComponents = app.getAvailableComponents();
		value_of(installedComponents).should_be_array();

		// check the components that we think are installed
		// first we verify that we have a list of components.
		for ( i =0; i < installedComponents.length; i++ )
		{
			var  item = installedComponents[i];
			// do we have a component?
			value_of(item).should_be_object();
			value_of(item.getName).should_be_function();
			value_of(item.getPath).should_be_function();
			value_of(item.getType).should_be_function();
			value_of(item.getVersion).should_be_function();
			value_of(item.isBundled).should_be_function();
			value_of(item.isLoaded).should_be_function();
			
			Titanium.API.info("processing installed component '" + item.getName() + 
							  "' type: '"+item.getType()+
							  "' version: '"+item.getVersion()+
							  "' path '" + item.getPath() + "'");
							  
			// installed components cannot be bundled.
			Titanium.API.info("item "+item.getName()+".isBundled() returned "+item.isBundled().toString());
		}
	},

	test_loaded_components: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();
		
		// get the components loaded by the application
		var loadedComponents = app.getComponents();
		var installedComponents = app.getAvailableComponents();
		
		value_of(loadedComponents).should_be_array();
		value_of(installedComponents).should_be_array();

		// now validate the loaded components and verify 
		// that these components are supposed to be here.
		// these components should be part of the bundle or installed or both.
		for ( i =0; i < loadedComponents.length; i++ )
		{
			var  item = loadedComponents[i];
			// do we have a component?
			value_of(item).should_not_be_null();
			value_of(item).should_be_object();
			value_of(item.getName).should_be_function();
			value_of(item.getPath).should_be_function();
			value_of(item.getType).should_be_function();
			value_of(item.getVersion).should_be_function();
			value_of(item.isBundled).should_be_function();
			value_of(item.isLoaded).should_be_function();
			
			// loaded components should always indicate so...
			value_of(item.isLoaded()).should_be_true();

			Titanium.API.info("processing loaded component '" + item.getName() + 
							  "' type: '"+item.getType()+
							  "' version: '"+item.getVersion()+
							  "' path '" + item.getPath() + "'");

			var name = item.getName();
			var path = item.getPath();
			var type = item.getType();
			var version = item.getVersion();
			
			// is this a valid component?  check against 
			// the installed components.
			for ( j=0; j< installedComponents.length; j++)
			{
				var obj = installedComponents[i];
				value_of(obj).should_be_object();

				if ( name == obj.getName() && 
					 path == obj.getPath() && 
					 version == obj.getVersion() ) 
				{
					value_of(obj.isLoaded()).should_be_true();
				}
			}
		}
	},

	test_bundled_components: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();
		
		// get the components loaded by the application
		var loadedComponents = app.getComponents();
		var bundledComponents = app.getBundledComponents();
		
		value_of(loadedComponents).should_be_array();
		value_of(bundledComponents).should_be_array();

		// check any components that are bundled with the app
		// first we verify that we have a list of components.
		for ( i =0; i < bundledComponents.length; i++ )
		{
			var  item = bundledComponents[i];
			// do we have a component?
			value_of(item).should_be_object();

			value_of(item.getName).should_be_function();
			value_of(item.getPath).should_be_function();
			value_of(item.getType).should_be_function();
			value_of(item.getVersion).should_be_function();
			value_of(item.isBundled).should_be_function();
			value_of(item.isLoaded).should_be_function();
			
			Titanium.API.info("processing bundled component '" + item.getName() + 
							  "' type: '"+item.getType()+
							  "' version: '"+item.getVersion()+
							  "' path '" + item.getPath() + "'");
			value_of(item.isBundled()).should_be_true();
		}
		
		// now validate the loaded components and verify 
		// that these components are supposed to be here.
		// these components should be part of the bundle or installed or both.
		for ( i =0; i < loadedComponents.length; i++ )
		{
			var  item = loadedComponents[i];
			// do we have a component?
			value_of(item).should_not_be_null();
			value_of(item).should_be_object();
			value_of(item.getName).should_be_function();
			value_of(item.getPath).should_be_function();
			value_of(item.getType).should_be_function();
			value_of(item.getVersion).should_be_function();
			value_of(item.isBundled).should_be_function();
			value_of(item.isLoaded).should_be_function();
			
			// loaded components should always indicate so...
			value_of(item.isLoaded()).should_be_true();

			Titanium.API.info("processing loaded component '" + item.getName() + 
							  "' type: '"+item.getType()+
							  "' version: '"+item.getVersion()+
							  "' path '" + item.getPath() + "'");

			var name = item.getName();
			var path = item.getPath();
			var type = item.getType();
			var version = item.getVersion();
			
			// is this a valid component?  check against 
			// the installed components.
			if ( bundledComponents.length != 0) 
			{
				for ( j=0; j< bundledComponents.length; j++)
				{
					var obj = bundledComponents[i];
					value_of(obj).should_be_object();

					if ( name == obj.getName() && 
						 path == obj.getPath() && 
						 version == obj.getVersion() ) 
					{
						value_of(obj.isBundled()).should_be_true();
					}
				}
			}
			else 
			{
				// we have not detected any bundled components
				// all components should have this flag as FALSE
				value_of(item.isBundled()).should_be_false();
			}
		}
	},

	// test the application dependancies API
	test_dependancies: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();

		var dependancies = app.getDependencies();
		value_of(dependancies).should_be_array();
		
		var components = app.getComponents();
		value_of(components).should_be_array();
						
		for (i=0; i<dependancies.length; i++ )
		{
			var dependancy = dependancies[i];
			
			value_of(dependancy).should_not_be_null();
		
			value_of(dependancy).should_be_object();
			value_of(dependancy.getName).should_be_function();
			value_of(dependancy.getType).should_be_function();
			value_of(dependancy.getVersion).should_be_function();
			
			Titanium.API.info("processing dependancy '" + dependancy.getName() + 
							  "' type: '"+dependancy.getType()+
							  "' version: '"+dependancy.getVersion()+"'");
			if (components)
			{
				// now that we have an object, check against the components list
				// each dependancy should be matched to a loaded component.
				for (j=0; j<components.length; j++ )
				{
					var obj = components[j];
					value_of(obj).should_be_object();
					
					var name = obj.getName();
					var type = obj.getType();
					var version = obj.getVersion();
					
					value_of(name).should_not_be_null();
					value_of(type).should_not_be_null();
					value_of(version).should_not_be_null();
					
					var bName = (name == dependancy.getName())?true:false;
					var bType = (type == dependancy.getType())?true:false;;
					var bVersion = (version == dependancy.getVersion())?true:false;;
					
					if ( bName && bType && bVersion)
					{
						Titanium.API.trace("match dependancy "+name+" to component");
						break;
					}
					
					// don't spam the log file
					if ( !bName || !bType || !bVersion)
					{
						Titanium.API.trace("match dependancy "+name+" to component");
					}
				}
			}
		}		
	},
	
	// test the installed runtime component functions
	test_runtime_component: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();

		// every module, runtime object and SDK should have a component entry		
		var components = app.getComponents();
		value_of(components).should_be_array();

		var runtime = app.getRuntime();
		// fail the test if we don't retrieve the list
		value_of(runtime).should_not_be_null();
		
		var bFound = false;
		
		Titanium.API.info("testing runtime component '" + runtime.getName() + 
						  "' type: '"+runtime.getType()+
						  "' version: '"+runtime.getVersion()+
						  "' path '" +runtime.getPath() + "'");
						  
		for ( i=0; i<components.length; i++ )
		{
			Titanium.API.info("processing component '" + components[i].getName() + 
							  "' type: '"+components[i].getType()+
							  "' version: '"+components[i].getVersion()+
							  "' path '" +components[i].getPath() + "'");
						 
			if ( runtime.getName() == components[i].getName() &&
				 runtime.getType() == components[i].getType() &&
				 runtime.getPath() == components[i].getPath())
			{
				Titanium.API.info("match runtime '" + runtime.getName() + "' to component entry");
				bFound = true;
				break;
			}
		}
		
		if ( !bFound )
		{
			Titanium.API.error("failed to match runtime object '" + runtime.getName() + "' to list of loaded components");
			value_of(bFound).should_be_true();
		}

	},
	
	// test the installed runtime component functions
	test_available_runtime_component: function()
	{
		Titanium.API.info("checking availableRuntimes[] ");
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();

		var runtime = app.getRuntime();
		value_of(runtime).should_not_be_null();

		var availableRuntimes = app.getAvailableRuntimes();
		// fail the test if we don't retrieve the list
		value_of(availableRuntimes).should_not_be_null();
		
		var bFound = false;

		for ( i=0; i<availableRuntimes.length; i++ )
		{
			Titanium.API.info("processing available runtime component '" + availableRuntimes[i].getName() + 
							  "' type: '"+availableRuntimes[i].getType()+
							  "' version: '"+availableRuntimes[i].getVersion()+
							  "' path '" + availableRuntimes[i].getPath() + "'");

			if ( runtime.getName() == availableRuntimes[i].getName() &&
				 runtime.getType() == availableRuntimes[i].getType() &&
				 runtime.getPath() == availableRuntimes[i].getPath())
			{
				Titanium.API.info("match runtime '" + runtime.getName() + "' to available Runtimes entry");
				bFound = true;
				break;
			}
		}
		if ( !bFound )
		{
			// fail the test if we don't find a value.						
			Titanium.API.error("failed to match runtime object '" + runtime.getName() + "' to list of available runtime objects");
			value_of(bFound).should_be_true();
		}
	},
	
	// test the installed runtime component functions
	test_bundled_runtime_component: function()
	{
		Titanium.API.info("checking bundledRuntimes[] ");

		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();

		var runtime = app.getRuntime();
		value_of(runtime).should_not_be_null();


		var bundledRuntimes = app.getBundledRuntimes();
		value_of(bundledRuntimes).should_be_array();

		Titanium.API.info("bundledRuntimes.length = "+bundledRuntimes.length);
		
		var bFound = false;
		
		for ( i=0; i<bundledRuntimes.length; i++ )
		{
			Titanium.API.info("processing bundled runtime component '" + bundledRuntimes[i].getName() + 
							  "' type: '"+bundledRuntimes[i].getType()+
							  "' version: '"+bundledRuntimes[i].getVersion()+
							  "' path '" + bundledRuntimes[i].getPath() + "'");
			if ( runtime.getName() == bundledRuntimes[i].getName() &&
				 runtime.getType() == bundledRuntimes[i].getType() &&
				 runtime.getPath() == bundledRuntimes[i].getPath())
			{
				Titanium.API.info("match runtime '" + runtime.getName() + "' to bundled Runtimes entry");
				bFound = true;
				break;
			}
		}
		if ( !bFound )
		{
			// fail the test if we don't find a value.						
			Titanium.API.error("failed to match runtime object '" + runtime.getName() + "' to list of bundled runtime objects");
			value_of(bFound).should_be_true();
		}
	},
	// test the installed modules component functions
	test_installed_modules: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();
		// every module, runtime object and SDK should have a component entry		
		var components = app.getComponents();
		value_of(components).should_be_array();
		
		var modules = app.getModules();
		value_of(modules).should_be_array();

		var bFound = false;
		for (j=0; j<modules.length; j++ )
		{
			var module = modules[j];
			
			value_of(module).should_not_be_null();
			if (!module)
			{
				Titanium.API.error("failed to retrieve module from modules["+j+"]");
			}
			else 
			{
				bFound = false;
				Titanium.API.info("processing module '" + module.getName() + 
								  "' type: '"+module.getType()+
								  "' version: '"+module.getVersion()+
								  "' path '" + module.getPath() + "'");

				for ( i=0; i<components.length; i++ )
				{
					if ( module.getName() == components[i].getName() &&
						 module.getType() == components[i].getType() &&
						 module.getPath() == components[i].getPath())
					{
						Titanium.API.trace("match module "+module.getName() +" to component entry");
						bFound = true;
						break;
					}
				}

				if ( !bFound )
				{
					// fail the test if we don't find a value.						
					Titanium.API.fatal("failed to match modules object to list of loaded components");
					value_of(bFound).should_be_true();
				}
			}
		}
	},
	// test the installed modules component functions
	test_installed_modules: function()
	{
		Titanium.API.info("checking bundledModules[] ");
		
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();
		var modules = app.getModules();
		value_of(modules).should_be_array();

		var bFound = false;
		
		var bundledModules = app.getBundledModules();
		value_of(bundledModules).should_be_array();
		
		var bFound = false;
		for (j=0; j<modules.length; j++ )
		{
			var module = modules[j];
			
			Titanium.API.info("processing module '" + module.getName() + 
							  "' type: '"+module.getType()+
							  "' version: '"+module.getVersion()+
							  "' path '" + module.getPath() + "'");

			for ( i=0; i<bundledModules.length; i++ )
			{
				var m = bundledModules[i];
				value_of(m).should_not_be_null();
				
				Titanium.API.info("processing bundled module component '" + m.getName() + 
								  "' type: '"+m.getType()+
								  "' version: '"+m.getVersion()+
								  "' path '" + m.getPath() + "'");

				if ( module.getName() == m.getName() &&
					 module.getType() == m.getType() &&
					 module.getPath() == m.getPath())
				{
					Titanium.API.trace("match bundled module "+module.getName() +" to modules entry");
					bFound = true;
					break;
				}
			}
		}
	},
	// test the installed modules component functions
	test_installed_modules: function()
	{
		Titanium.API.info("checking availableModules[] ");
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();
		var modules = app.getModules();
		value_of(modules).should_be_array();

		var bFound = false;

		var availableModules = app.getAvailableModules();
		value_of(availableModules).should_be_array();

		for (j=0; j<modules.length; j++ )
		{
			var module = modules[j];
			
			Titanium.API.info("processing module component '" + module.getName() + 
							  "' type: '"+module.getType()+
							  "' version: '"+module.getVersion()+
							  "' path '" + module.getPath() + "'");

			for ( i=0; i<availableModules.length; i++ )
			{
				var m = availableModules[i];
				value_of(m).should_not_be_null();
				
				Titanium.API.info("processing available module component '" + m.getName() + 
								  "' type: '"+m.getType()+
								  "' version: '"+m.getVersion()+
								  "' path '" + m.getPath() + "'");

				if ( module.getName() == m.getName() &&
					 module.getType() == m.getType() &&
					 module.getPath() == m.getPath())
				{
					Titanium.API.trace("match avialable module "+module.getName() +" to modules entry");
					bFound = true;
					break;
				}
			}
			if ( !bFound )
			{
				Titanium.API.fatal("failed to match module "+module.getName() +" to list of available modules");
				// fail the test if we don't find a value.						
				value_of(bFound).should_be_true();
			}
		}
	},

	// test the API SDK functions
	test_api_sdk: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();

		// every module, runtime object and SDK should have a component entry		
		var components = app.getComponents();
		value_of(components).should_be_object();
		
		// once we have the list of components from the application 
		// object, we need to  verify that the lists are the same 
		// that we get through the application object.

		Titanium.API.info("checking installed SDKs[] ");
		
		var sdks = Titanium.API.getInstalledSDKs();
		value_of(sdks).should_be_array();
		
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
	},

	// test the installed modules component functions
	test_installed_mobile_sdks: function()
	{
		// test the objects in API		
		var app = Titanium.API.getApplication();
		value_of(app).should_be_object();

		// every module, runtime object and SDK should have a component entry		
		var components = app.getComponents();
		value_of(components).should_be_object();

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
		value_of(app).should_be_object();
		
		var argv = app.getArguments();

		value_of(argv).should_be_array();
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
	test_api_application_Datapath: function()
	{
		// get the application object
		var app = Titanium.API.getApplication();
		value_of(app).should_not_be_null();
		
		var dataPath = app.getDataPath();
		value_of(dataPath).should_not_be_null();
		Titanium.API.info("data path = "+dataPath);
	},
	
	test_api_application_Arguments: function()
	{
		// get the application object
		var app = Titanium.API.getApplication();
		value_of(app).should_not_be_null();

		var argv = app.getArguments();
		value_of(argv).should_not_be_null();

		if ( argv )
		{
			var exePath = app.getExecutablePath();
			
			value_of(exePath).should_not_be_null();
			
			// argv[0] is the fully qualified name and path to the exe
			var index = argv[0].indexOf(exePath);
			value_of(index).should_not_be(-1);			
		}
	},
	
	test_api_application_manifest_path: function()
	{
		// get the application object
		var app = Titanium.API.getApplication();
		value_of(app).should_not_be_null();

		var manifestPath = app.getManifestPath();
		value_of(manifestPath).should_not_be_null();
		Titanium.API.info("application Manifest path = "+manifestPath);
		
	},
	
	test_api_application_path: function()
	{
		// get the application object
		var app = Titanium.API.getApplication();
		value_of(app).should_not_be_null();

		var path = app.getPath();
		value_of(path).should_not_be_null();
		Titanium.API.info("application path = "+path);	
	},

	// test the manifest functions
	test_api_manifest: function()
	{
		// get the application object
		var app = Titanium.API.getApplication();
		value_of(app).should_not_be_null();

		var manifest  = app.getManifest();
		value_of(manifest).should_be_object();
		value_of(manifest.length).should_not_be(0);
		
		for (i=0; i< manifest.length; i++ )
		{
			value_of(manifest[i].length).should_be(2);
			value_of(manifest[i][0]).should_be_string();
			value_of(manifest[i][1]).should_be_string();
		}
	},
	
	test_api_app_version: function()
	{
		// get the application object
		var app = Titanium.API.getApplication();
		value_of(app).should_not_be_null();
		
		var ver = app.getVersion();
		value_of(ver).should_be_string();
		
		// this is the version for the test harness app, not drillbit.
		value_of(ver).should_be("0.5.0");
	},
	
	test_api_app_GUID: function()
	{
		// get the application object
		var app = Titanium.API.getApplication();
		value_of(app).should_not_be_null();
		
		var guid = app.getGUID();
		value_of(guid).should_be_string();
		
		// this is the GUID for the test harness app, not drillbit.
		value_of(guid).should_be(Titanium.App.getGUID());
	},
	
	test_api_app_ID: function()
	{
		// get the application object
		var app = Titanium.API.getApplication();
		value_of(app).should_not_be_null();
		
		var appid = app.getID();
		value_of(appid).should_be_string();
		
		// this is the ID for the test harness app, not drillbit.
		value_of(appid).should_be(Titanium.App.getID());
	}
	
});
