describe("ti.Api tests",
{
	// test the logging functions
	test_logging_methods: function()
	{
		value_of(Titanium.API.critical).should_be_function();
		value_of(Titanium.API.debug).should_be_function();
		value_of(Titanium.API.error).should_be_function();
		value_of(Titanium.API.fatal).should_be_function();
		value_of(Titanium.API.log).should_be_function();
		value_of(Titanium.API.notice).should_be_function();
		value_of(Titanium.API.trace).should_be_function();
		value_of(Titanium.API.warn).should_be_function();

		Titanium.API.critical("this is a critical message");
        Titanium.API.debug("this is a debug message");
		Titanium.API.error("this is a error message");
		Titanium.API.fatal("this is a fatal message");
		Titanium.API.log(1,"this is a log message");
		Titanium.API.notice("this is a notice message");
		Titanium.API.trace("this is a trace message");
		Titanium.API.warn("this is a warn message");
	},
	validate_properties: function()
	{
	    // get the platform and version.
		value_of(Titanium.platform).should_be_one_of(['win32','osx','linux']);
		value_of(Titanium.version).should_be_string();

		value_of(Titanium.API.APP_UPDATE).should_not_be_null();
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
		value_of(Titanium.API.fire).should_be_function();
		value_of(Titanium.API.get).should_be_function();
		value_of(Titanium.API.getApplication).should_be_function();
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
		value_of(Titanium.API.register).should_be_function();
		value_of(Titanium.API.set).should_be_function();
		value_of(Titanium.API.trace).should_be_function();
		value_of(Titanium.API.unregister).should_be_function();
		value_of(Titanium.API.warn).should_be_function();
	},
    // test the application components API
	test_components: function()
	{
        // test the objects in API		
        var app = Titanium.API.getApplication();
      
        value_of(app).should_not_be_null();
        if ( app ) 
        {
		    value_of(app).should_be_object();
            // get the components loaded by the application
            var loadedComponents = app.getComponents();
            var installedComponents = app.getAvailableComponents();
            var bundledComponents = app.getBundledComponents();
            
            
            value_of(loadedComponents).should_not_be_null();
            value_of(installedComponents).should_not_be_null();
            value_of(bundledComponents).should_not_be_null();

            if ( loadedComponents && installedComponents && bundledComponents ) {
                // check any components that are bundled with the app
                // first we verify that we have a list of components.
                for ( i =0; i < bundledComponents.length; i++ )
                {
                    var  item = bundledComponents[i];
                    // do we have a component?
		            value_of(item).should_be_object();
		            if ( item ) 
		            {
		                value_of(item.getManifest).should_be_function();
		                value_of(item.getName).should_be_function();
		                value_of(item.getPath).should_be_function();
		                value_of(item.getType).should_be_function();
		                value_of(item.getVersion).should_be_function();
		                value_of(item.isBundled).should_be_function();
		                value_of(item.isLoaded).should_be_function();
		            }
                }
                
                // check the components that we think are installed
                // first we verify that we have a list of components.
                for ( i =0; i < installedComponents.length; i++ )
                {
                    var  item = installedComponents[i];
                    // do we have a component?
		            value_of(item).should_be_object();
		            if ( item ) 
		            {
		                value_of(item.getManifest).should_be_function();
		                value_of(item.getName).should_be_function();
		                value_of(item.getPath).should_be_function();
		                value_of(item.getType).should_be_function();
		                value_of(item.getVersion).should_be_function();
		                value_of(item.isBundled).should_be_function();
		                value_of(item.isLoaded).should_be_function();
		            }
                }
                
                // now validate the loaded components and verify 
                // that these components are supposed to be here.
                // these components should be part of the bundle or installed or both.
                for ( i =0; i < loadedComponents.length; i++ )
                {
                    var  item = loadedComponents[i];
                    // do we have a component?
		            value_of(item).should_be_object();
		            if ( item ) 
		            {
		                value_of(item.getManifest).should_be_function();
		                value_of(item.getName).should_be_function();
		                value_of(item.getPath).should_be_function();
		                value_of(item.getType).should_be_function();
		                value_of(item.getVersion).should_be_function();
		                value_of(item.isBundled).should_be_function();
		                value_of(item.isLoaded).should_be_function();
    		            
		                // loaded components should always indicate so...
		                value_of(item.isLoaded()).should_be_true();

                        var manifest = item.getManifest();
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
		                        if ( obj ) 
		                        {
		                            if ( name == obj.getName() && 
		                                 path == obj.getPath() && 
		                                 version == obj.getVersion() ) 
		                            {
		                                value_of(obj.isBundled()).should_be_true();
		                            }
		                        }
		                    }
    		            
		                }
		                else {
		                    // we have not detected any bundled components
		                    // all components should have this flag as FALSE
		                    value_of(item.isBundled()).should_be_false();
		                }
		                // is this a valid component?  check against 
		                // the installed components.
		                for ( j=0; j< installedComponents.length; j++){
		                    var obj = installedComponents[i];
	                        value_of(obj).should_be_object();
	                        if ( obj ) {
	                            if ( name == obj.getName() && 
	                                 path == obj.getPath() && 
	                                 version == obj.getVersion() ) {
	                                value_of(obj.isLoaded()).should_be_true();
	                                if ( !obj.isLoaded() )
	                                {
	                                    Titanium.API.error("obj " + obj.getName() + " is marked as not loaded");
	                                }
	                            }
	                        }
		                }
		            }
                }
            }
		}
	},
	// test the application dependancies API
	test_dependancies: function()
	{
        // test the objects in API		
        var app = Titanium.API.getApplication();
      
        value_of(app).should_not_be_null();
        if ( app ) 
        {
		    value_of(app).should_be_object();
		    var dependancies = app.getDependencies();
		    
		    // retrieve our list of depenancies.  can be NULL
		    if ( dependancies ) 
		    {
		        var components = app.getComponents();
		        
		        if ( !components )
		        {
		            Titanium.API.error("failed to retrieve the loaded components list.");
		        }
		        else 
		        {
    		        value_of(components).should_be_object();
		        }
		        
		        value_of(dependancies).should_be_object();
    		    
    		    for (i=0; i<dependancies; i++ )
    		    {
    		        var dependancy = dependancies[i];
    		        
    		        if( dependency )
    		        {
		                value_of(dependency).should_be_object();
		                value_of(dependency.getName).should_be_function();
		                value_of(dependency.getType).should_be_function();
		                value_of(dependency.getVersion).should_be_function();
		                
		                if (components)
		                {
		                    // now that we have an object, check against the components list
		                    // each dependancy should be matched to a loaded component.
		                    for (j=0; j<components.length; j++ )
		                    {
    		                    var obj = components[j];
    		                    if ( obj )
    		                    {
    		                        value_of(obj).should_be_object();
    		                        
    		                        var name = obj.getName();
    		                        var type = obj.getType();
    		                        var version = obj.getVersion();
    		                        
    		                        if ( name && type && version )
    		                        {
    		                            var bName = (name == dependancy.getName())?true:false;
    		                            var bType = (type == dependancy.getType())?true:false;;
    		                            var bVersion = (version == dependancy.getVersion())?true:false;;
    		                            
    		                            if ( bName && bType && bVersion)
    		                            {
    		                                Titanium.API.trace("match dependancies to component");
    		                                break;
    		                            }
    		                            
    		                            if ( bName )
    		                                 Titanium.API.trace("matched dependancy name to component");
    		                            else Titanium.API.error("failed to match dependancy name to component");
    		                            
    		                            if ( bType )
    		                                 Titanium.API.trace("matched dependancy type to component");
    		                            else Titanium.API.error("failed to match dependancy type to component");
    		                                
    		                            if ( bVersion )
    		                                 Titanium.API.trace("matched dependancy version to component");
    		                            else Titanium.API.error("failed to match dependancy version to component");
    		                        }
    		                        else
    		                        {
    		                            Titanium.API.fatal("failed to retrieve component infomation in dependancy check.");
    		                        }
    		                    }
		                    }
		                }
    		        }
    		    }		
		    }
		    else
		    {
		        Titanium.API.warn("no dependancies detected");
		    }
		}
		// create a dependancy
	    var dep = Titanium.API.createDependency(Titanium.API.RUNTIME, "test", "0.0.1", Titanium.API.EQ);
	}
});
