describe("ti.Api tests",
{
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

		// Titanium.API.Component should be an instance
		// value_of(Titanium.API.Component).should_be_object();
		// value_of(Titanium.API.Component.getManifest).should_be_function();
		// value_of(Titanium.API.Component.getName).should_be_function();
		// value_of(Titanium.API.Component.getPath).should_be_function();
		// value_of(Titanium.API.Component.getType).should_be_function();
		// value_of(Titanium.API.Component.getVersion).should_be_function();
		// value_of(Titanium.API.Component.isBundled).should_be_function();
		// value_of(Titanium.API.Component.isLoaded).should_be_function();

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
	test_dependancies: function()
	{
		var dep = Titanium.API.createDependency(Titanium.API.MODULE, "blahblah", "0.4");
		value_of(dep).should_be_object();
		value_of(dep.getName).should_be_function();
		value_of(dep.getType).should_be_function();
		value_of(dep.getVersion).should_be_function();
		value_of(dep.getName()).should_be("blahblah");
		value_of(dep.getType()).should_be(Titanium.API.MODULE);
		value_of(dep.getVersion()).should_be("0.4");

	}
});
