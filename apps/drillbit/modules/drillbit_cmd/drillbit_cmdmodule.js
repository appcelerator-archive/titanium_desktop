(function(){
	var ti = Titanium;
	var tiFS = ti.Filesystem;
	var print = ti.API.print;
	var println = ti.App.stdout;
	var errPrint = ti.App.stderr;
	
	var frontend = {
		passed: 0, failed: 0,
		error: function(msg) {
			errPrint(msg);
		},
		suite_started: function(suite)
		{
			println("Testing suite '" + suite + "'...");
		},
		show_current_test: function(suite, test)
		{
			print("\tRunning "+ test + "...");
		},
		test_passed: function(suite, test)
		{
			println(" passed");
			this.passed++;
		},
		test_failed: function(suite, test, error)
		{
			println(" failed");
			println("\t\tError: " + error);
			this.failed++;
		},
		suite_finished: function(suite)
		{
			println("Finished '" + suite + "'");
		},
		all_finished: function()
		{
			println("Total: " + this.passed + " passed, " + this.failed + " failed");
		}
	};
	
	var test_files = [];
	for (var c=0;c<ti.App.arguments.length;c++)
	{
		var arg = ti.App.arguments[c];
		
		if (arg == '--debug-tests')
		{
			ti.Drillbit.debug_tests = true;
		}
		else
		{
			var file = tiFS.getFile(arg);
			if (!file.exists())
			{
				errPrint("Warning: " + arg + " doesn't exist, skipping.");
			}
			else
			{
				println("Loading file: " + file.nativePath());
				test_files.push(file);
			}
		}
	}
	
	if (test_files.length == 0)
	{
		errPrint("Usage:\n\t drillbit_cmd.py [--debug-tests] test [test2..testN]");
		ti.App.exit();
	}
	else {
		ti.Drillbit.frontend = frontend;
		ti.Drillbit.auto_close = true;
		ti.Drillbit.loadTests(test_files);
		ti.Drillbit.setupTestHarness(tiFS.getFile(tiFS.getApplicationDirectory(), 'manifest_harness'));
		ti.Drillbit.runTests();
	}
})();