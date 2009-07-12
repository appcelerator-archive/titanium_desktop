(function(){
	function require(app_url) {
		var fs = Titanium.Filesystem.getFileStream(Titanium.App.appURLToPath(app_url));
		fs.open(Titanium.Filesystem.MODE_READ);
		var c = fs.read();
		fs.close();
		eval(String(c));
	}

	require("app://js/app.js");
	require("app://js/project.js");
	
	//Titanium.API.setLogLevel(Titanium.API.LERROR);
	
	var test_files = [];
	for (var c=0;c<Titanium.App.arguments.length;c++)
	{
		var arg = Titanium.App.arguments[c];
		
		if (arg == '--debug-tests')
		{
			Titanium.Drillbit.debug_tests = true;
		}
		else
		{
			var file = Titanium.Filesystem.getFile(arg);
			if (!file.exists())
			{
				Titanium.App.stdout("Error, " + arg + " doesn't exist!");
				Titanium.App.exit();
			}
			
			Titanium.App.stdout("Loading file: " + file.nativePath());
			test_files.push(file);
		}
	}
	
	if (test_files.length == 0)
	{
		Titanium.App.stdout("Error, Usage: " + Titanium.Process.getCurrentProcess().getArguments()[0] + " test [test2 test3..]");
		Titanium.App.exit();
	}
	else {
		Titanium.Drillbit.loadTests(test_files);
		Titanium.Drillbit.setupTestHarness(
			Titanium.Filesystem.getFile(Titanium.Filesystem.getApplicationDirectory(),'harness_manifest'));
		Titanium.Drillbit.runTests();
	}
})();