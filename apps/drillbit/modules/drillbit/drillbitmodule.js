(function() {
	var Drillbit = function() {
		var TFS = Titanium.Filesystem;
		var TA  = Titanium.App;
		
		this.frontend = null;
		this.auto_close = false;
		this.debug_tests = false;
		this.run_tests_async = false;
		this.window = null;
		
		this.tests = {};
		this.test_names = [];
		this.total_assertions = 0;
		this.total_tests = 0;
		this.total_files = 0;
		this.current_test = null;
		this.tests_started = 0;
		this.test_duration = 0;
		
		var current_test_load = null;
		var excludes = ['before','before_all','after','after_all','timeout'];
		var running_tests = 0;
		var running_completed = 0;
		var running_passed = 0;
		var running_failed = 0;
		var test_failures = false;
		var specific_tests = null;
		var executing_tests = [];
		
		this.results_dir = TFS.getFile(TA.appURLToPath('app://test_results'));
		var app_dir = TFS.getApplicationDirectory();
		var drillbit_funcs = TFS.getFile(TA.appURLToPath('app://drillbit_func.js')).read();
		var user_scripts_dir = null;
		var app = Titanium.API.getApplication();
		var tiapp_backup = null, tiapp = null;
		var manifest_backup = null, manifest = null;
		var non_visual_ti = null;
		var self = this;
		

		this.require = function(app_url) {
			this.include(TA.appURLToPath(app_url));
		}

		this.include = function(path)
		{
			var code = TFS.getFile(path).read();
			try {
				with (this) {
					eval(code.toString());
				}
			} catch (e) {
				Titanium.App.stdout("Error: "+String(e)+", "+path+", line:"+e.line);
			}
		};
		
		this.frontend_do = function()
		{
			try {
				var args = Array.prototype.slice.call(arguments);
			
				var fn_name = args[0];
				args.shift();
			
				if (this.frontend &&
					fn_name in this.frontend && typeof this.frontend[fn_name] == 'function')
				{
					this.frontend[fn_name].apply(this.frontend, args);
				}
			}
			catch (e)
			{
				Titanium.App.stderr("Error: " +e);
			}
		}
		
		function findLine(needle,haystack)
		{
			var lines = haystack.split('\n');
			for (var i = 0; i < lines.length; i++)
			{
				if (needle.test(lines[i]))
				{
					if (/^[\t ]*{[\t ]*$/.test(lines[i+1]))
					{
						//offset by 1 when the bracket is on a seperate line
						// Function.toString show an inline bracket, so we need to compensate
						return i+1;
					}
					return i;
				}
			}
			return -1;
		}
		
		function describe(description,test)
		{
			current_test_load.description = description;
			current_test_load.test = test;
			current_test_load.line_offsets = {};
			current_test_load.timeout = test.timeout || 5000;
			current_test_load.assertions = {};
			current_test_load.assertion_count = 0;
			current_test_load.source_file = TFS.getFile(current_test_load.dir, current_test_load.name+".js");
			var testSource = current_test_load.source_file.read().toString();
			
			for (var p in test)
			{
				if (excludes.indexOf(p)==-1)
				{
					var fn = test[p];
					if (typeof fn == 'function')
					{
						self.total_tests++;
						current_test_load.assertion_count++;
						current_test_load.assertions[p]=false;
						var r = new RegExp(p+" *: *function *\\(");
						current_test_load.line_offsets[p] = findLine(r,testSource);
					}
				}
			}

			self.total_files++;
			current_test_load = null;
		};
		
		this.loadTestFile = function(test_file)
		{
			var name = test_file.name();
			var ext = test_file.extension();
			name = name.replace('.'+ext,'');
			var dir = test_file.parent();
			var jsfile = TFS.getFile(dir,name+'.js');
			if (!jsfile.exists() || dir.name() != name)
			{
				return;
			}
			var entry = this.tests[name];
			if (!entry)
			{
				entry = {name:name,dir:dir};
				this.tests[name] = entry;
				this.test_names.push(name);
			}
			entry[ext] = test_file;
			current_test_load = entry;
			try
			{
				eval(String(jsfile.read()));
			}
			catch(EX)
			{
				this.frontend_do('error', "error loading: "+test_file+". Exception: "+EX+" (line: "+EX.line+")");
			}
		};
		
		this.loadTestDir = function(test_dir)
		{
			var dirname = test_dir.name();
			var test_file = TFS.getFile(test_dir, dirname+".js");
			if (test_file.exists()) {
				this.loadTestFile(test_file);
			}
		};
		
		this.loadTests = function(test_files)
		{
			this.results_dir.createDirectory();

			var f = Titanium.Filesystem.getFile(this.results_dir, "results.html");
			if (f.exists()) {
				f.deleteFile();
			}
		
			for (var c=0;c<test_files.length;c++)
			{
				var file = TFS.getFile(test_files[c]);
				if (file.isDirectory())
				{
					this.loadTestDir(file);
				}
				else
				{
					this.loadTestFile(file);
				}
			}

			this.test_names.sort();
		};
	
		this.setupTestHarness = function(harness_manifest)
		{
			this.require('app://js/app.js');
			this.require('app://js/project.js');
			// create app structure
			app = Titanium.createApp(
				Titanium.API.application.runtime, // Runtime component
				Titanium.API.application.getResourcesPath(), // Stage in Resources directory
				'test_harness', // app name
				'CF0D2CB7-B4BD-488F-9F8E-669E6B53E0C4', // app guid
				false);
			tiapp_backup = TFS.getFile(app.base,'_tiapp.xml');
			manifest_backup = TFS.getFile(app.base,'_manifest');

			//var mymanifest = TFS.getFile(TFS.getApplicationDirectory(),'manifest');
			manifest = TFS.getFile(app.base,'manifest');
			manifest.write(harness_manifest.read().toString());
			
			tiapp_harness = TFS.getFile(TFS.getApplicationDirectory(), 'tiapp_harness.xml');
			tiapp = TFS.getFile(app.base, 'tiapp.xml');
			tiapp_harness.copy(tiapp);
			tiapp.copy(tiapp_backup);
			manifest.copy(manifest_backup);

			var ti_contents = String(tiapp.read());
			non_visual_ti = ti_contents.replace('<visible>true</visible>','<visible>false</visible>');

			// copy in our user script which is the driver
			user_scripts_dir = TFS.getFile(app.resources,'userscripts');
			user_scripts_dir.createDirectory();
		};
	
		this.runTests = function(tests_to_run)
		{
			if (!tests_to_run)
			{
				tests_to_run = [];
				for (var i = 0; i < this.test_names.length; i++)
				{
					tests_to_run.push({suite: this.test_names[i], tests:'all'});
				}
			}
			
			for (var i = 0; i < tests_to_run.length; i++)
			{
				var name = tests_to_run[i].suite;
				var entry = this.tests[name];
				entry.tests_to_run = tests_to_run[i].tests;
				
				executing_tests.push(entry);
				running_tests+=entry.assertion_count;
			}
		
			this.tests_started = new Date().getTime();
			if (this.run_tests_async)
			{
				this.window.setTimeout(function(){self.run_next_test();}, 1);
			}
			else
			{
				this.run_next_test();
			}
		};
	
		this.run_test = function(entry)
		{
			var dir = app.resources;
		
			// make sure we cleanup
			var list = user_scripts_dir.getDirectoryListing();
			for (var c=0;c<list.length;c++)
			{
				var lf = list[c];
				if (lf.isFile())
				{
					lf.deleteFile();
				}
			}
		
			// we always initially override
			tiapp_backup.copy(tiapp);
			manifest_backup.copy(manifest);

			// make sure we have an index file always
			var tofile = TFS.getFile(dir,'index.html');
			var html = '<html><head><script type="text/javascript"></script></head><body>Running...'+entry.name+'</body></html>';
			tofile.write(html);

			var html_found = false;
			var tiapp_found = false;
			function strip_extension(f)
			{
				var name = f.name();
				return name.replace('.'+f.extension(),'');
			}

			var files = entry.dir.getDirectoryListing();
			for (var c=0;c<files.length;c++)
			{
				var src = files[c];
				var same_as_testname = strip_extension(src) == entry.name;
				if (src.name() == entry.name+'.js')
				{
					continue;
				}
				if (same_as_testname)
				{
					var ext = src.extension();
					switch(ext)
					{
						case 'xml':
						{
							tiapp_found=true;
							tiapp.write(src.read());
							break;
						}
						case 'html':
						{
							var tofile = TFS.getFile(dir,'index.html');
							src.copy(tofile);
							html_found = true;
							break;
						}
						case 'usjs':
						{
							var tofile = TFS.getFile(user_scripts_dir,entry.name+'.js');
							src.copy(tofile);
							break;
						}
						case 'manifest':
						{
							var tofile = TFS.getFile(app.base,'manifest');
							src.copy(tofile);
							break;
						}
						default:
						{
							// just copy the file otherwise
							Titanium.API.debug("copying "+src+" to "+dir);
							src.copy(dir);
							break;
						}
					}
				}
				else
				{
					// just copy the file otherwise
					src.copy(dir);
				}
			}

			// make it non-visual if no HTML found
			if (!html_found && !tiapp_found)
			{
				tiapp.write(non_visual_ti);
			}
			
			var modules = Titanium.API.getApplication().getModules();
			var module = null;
			for (var i = 0; i < modules.length; i++)
			{
				if (modules[i].getName() == "drillbit") {
					module = modules[i]; 
					break;
				}
			}
			
			var file = TFS.getFile(module.getPath(), "ejs.js");
			var template = TFS.getFile(module.getPath(), "template.js").read().toString();
			
			this.include(file.nativePath());
			var runner_js = TFS.getFile(user_scripts_dir,entry.name+'_driver.js');
			var data = {entry: entry, Titanium: Titanium, excludes: excludes};
			var user_script = null;
			
			try {
				user_script = new EJS({text: template, name: "template.js"}).render(data);
			} catch(e) {
				this.frontend_do('error',"Error rendering template: "+e+",line:"+e.line);
			}
			
			TFS.getFile(module.getPath(),"template_out.js").write(user_script);
			runner_js.write(user_script);

			var profile_path = TFS.getFile(this.results_dir,entry.name+'.prof');
			var log_path = TFS.getFile(this.results_dir,entry.name+'.log');

			profile_path.deleteFile();
			log_path.deleteFile();

			var args = [app.executable.nativePath(), '--profile="'+profile_path+'"']
			args.push('--logpath="'+log_path+'"')
			args.push('--bundled-component-override="'+app_dir+'"')
			args.push('--no-console-logging');
			args.push('--debug');
			if (this.debug_tests) {
				args.push('--attach-debugger');
			}

			args.push('--results-dir="' + this.results_dir + '"');
			var process = Titanium.Process.createProcess(args);
			Titanium.App.stdout("running: " + process);
			var passed = 0;
			var failed = 0;
			process.setOnReadLine(function(data)
			{
				var i = data.indexOf('DRILLBIT_');
				if (i != -1)
				{
					if (data.indexOf('DRILLBIT_TEST:') != -1) {
						var comma = data.indexOf(',');
						var suite_name = data.substring(15,comma);
						var test_name = data.substring(comma+1);
						self.frontend_do('show_current_test', suite_name, test_name);
						return;
					}
					else if (data.indexOf('DRILLBIT_ASSERTION:') != -1) {
						var comma = data.indexOf(',');
						var test_name = data.substring('DRILLBIT_ASSERTION:'.length+1, comma);
						var line_number = data.substring(comma+1);
						self.total_assertions++;
						self.frontend_do('add_assertion', test_name, line_number);
						return;
					}

					var test_name = data.substring(15);
					var test_passed = data.indexOf('_PASS:')!=-1;
					running_completed++;
					if (test_passed) {
						passed++; running_passed++;
						self.frontend_do('test_passed', entry.name, test_name);
					}
					else {
						failed ++; running_failed++;
						var dashes = test_name.indexOf(" --- ");
						var error = test_name.substring(dashes+5);
						var test_args = test_name.substring(0,dashes).split(',');
						test_name = test_args[0];
						line_number = test_args[1];
						self.frontend_do('test_failed', entry.name, test_name, line_number, error);
					}

					self.frontend_do('total_progress', running_passed, running_failed, self.total_tests);

					var msg = "Completed: " +entry.name + " ... " + running_completed + "/" + running_tests;
					self.frontend_do('update_status', msg);
				}
				else
				{
					self.frontend_do('process_data', data);
				}
			});
			var size = 0;
			var timer = null;
			var start_time = new Date().getTime();
			var original_time = start_time;

			// start a stuck process monitor in which we check the 
			// size of the profile file -- if we're not doing anything
			// we should have a file that hasn't changed in sometime
			// TODO we need a way to monitor from cmdline, though it probably
			// isn't as important there
			if (this.window) {
				timer = this.window.setInterval(function()
				{
					var t = new Date().getTime();
					var newsize = profile_path.size();
					var timed_out = (t-original_time) > 40000;
					if (newsize == size || timed_out)
					{
						if (timed_out || t-start_time>=entry.timeout)
						{
							this.window.clearInterval(timer);
							this.current_test.failed = true;
							update_status(this.current_test.name + " timed out");
							test_status(this.current_test.name,'Failed');
							process.terminate();
							return;
						}
					}
					else
					{
						size = newsize;
					}
					start_time = t;
				},1000);
			}

			process.addEventListener(Titanium.EXIT, function(event)
			{
				self.frontend_do('suite_finished', self.current_test.name);
				try
				{
					if (this.window) this.window.clearInterval(timer);
					if (!self.current_test.failed)
					{
						var r = TFS.getFile(self.results_dir,self.current_test.name+'.json').read();
						var rs = '(' + r + ');';
						var results = eval(rs);
						self.current_test.results = results;
						self.frontend_do('test_status', self.current_test.name,results.failed>0?'Failed':'Passed');
						self.frontend_do('update_status', self.current_test.name + ' complete ... '+results.passed+' passed, '+results.failed+' failed');
						if (!test_failures && results.failed>0)
						{
							test_failures = true;
						}
					}
					else
					{
						test_failures = true;
					}
				}
				catch(E)
				{
					self.frontend_do('error', "onexit failure = "+E+" at "+E.line);
				}
				self.run_next_test();
			});
			process.launch();
		};
	
		this.run_next_test = function()
		{
			if (executing_tests==null || executing_tests.length == 0)
			{
				this.test_duration = (new Date().getTime() - this.tests_started)/1000;
				this.frontend_do('all_finished');
				executing_tests = null;
				this.current_test = null;
				self.frontend_do('update_status', 'Testing complete ... took ' + this.test_duration + ' seconds',true);
				var f = TFS.getFile(this.results_dir,'drillbit.json');
				f.write("{\"success\":" + String(!test_failures) + "}");
				if (self.auto_close)
				{
					Titanium.App.exit(test_failures ? 1 : 0);
				}
				return;
			}
			var entry = executing_tests.shift();
			this.current_test = entry;
			this.current_test.failed = false;
			self.frontend_do('update_status', 'Executing: '+entry.name+' ... '+running_completed + "/" + running_tests);
			self.frontend_do('suite_started', entry.name);
			this.run_tests_async ? this.window.setTimeout(function(){self.run_test(entry)},1) : this.run_test(entry);
		};
		
		this.reset = function()
		{
			executing_tests = [];
			running_tests = 0;
			running_completed = 0;
			running_passed = running_failed = this.total_assertions = 0;
		}
	};
	
	Titanium.Drillbit = new Drillbit();
})();
