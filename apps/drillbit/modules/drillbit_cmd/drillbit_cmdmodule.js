(function(){
	var ti = Titanium;
	var tiFS = ti.Filesystem;
	var print = ti.API.print;
	var println = ti.App.stdout;
	var errPrint = ti.App.stderr;
	
	// Javascript with ANSI color, this might be a first.
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
			print("  Running '"+ test + "'...");
		},
		test_passed: function(suite, test)
		{
			println(" [32m[1mpassed[0m");
			this.passed++;
		},
		test_failed: function(suite, test, line_number, error)
		{
			println(" [31m[1mfailed[0m");
			println("   => " + test + "() line " + line_number);
			println("   => " + error);
			this.failed++;
		},
		suite_finished: function(suite)
		{
			println("Finished '" + suite + "'");
		},
		all_finished: function()
		{
			println("Total: " + this.passed + " passed, " + this.failed + " failed, "
				+ ti.Drillbit.assertion_count + " assertions");
		}
	};
	
	var tests = [];
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
			var tokens = arg.split(':');
			var fname = tokens[0];
			var testname = tokens[1] || null;
			var file = tiFS.getFile(fname);
			
			if (!file.exists())
			{
				var src_file = null;
				if (Titanium.platform=="osx") {
					src_file = tiFS.getFile(ti.API.getApplication().getPath(), '..', '..', "..", "..",
						'apps', 'drillbit', 'Resources', 'tests', fname, fname+'.js');
				} else {
					src_file = tiFS.getFile(ti.API.getApplication().getPath(), '..', "..", "..",
						'apps', 'drillbit', 'Resources', 'tests', fname, fname+'.js');
				}
				if (src_file.exists())
				{
					var t = 'all'
					if (testname != null) t = testname.split(',');
					
					test_files.push(src_file);
					tests.push({'suite':fname, tests:t});
				}
				else errPrint("Warning: " + arg + " doesn't exist, skipping.");
			}
			else
			{
				var suite = file.name().substring(0,file.name().length-3);
				println("Loading suite: " + suite + ", file: " + file.nativePath());
				test_files.push(file);
				tests.push({'suite':suite, tests:'all'});
			}
		}
	}
	
	if (tests.length == 0)
	{
		errPrint("Usage:\n\t drillbit_cmd.py [--debug-tests] suite[:test_name] [suite2..suiteN]");
		ti.App.exit();
	}
	else {
		ti.Drillbit.frontend = frontend;
		ti.Drillbit.auto_close = true;
		ti.Drillbit.loadTests(test_files);
		ti.Drillbit.setupTestHarness(tiFS.getFile(tiFS.getApplicationDirectory(), 'manifest_harness'));
		println("running tests: "+tests[0].tests);
		ti.Drillbit.runTests(tests);
	}
})();