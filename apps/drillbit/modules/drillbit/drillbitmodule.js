(function() {
	var Drillbit = function(frontend) {
		var TFS = Titanium.Filesystem;
		var TA  = Titanium.App;
		this.frontend = frontend;
		this.auto_close = false;
		this.debug_tests = false;
		
		var current_test_load = null;
		var current_test = null;
		var excludes = ['before','before_all','after','after_all','timeout'];
		var total_tests = 0;
		var total_files = 0;
		var running_tests = 0;
		var running_completed = 0;
		var running_passed = 0;
		var running_failed = 0;
		var test_failures = false;
		var specific_tests = null;
		var total_assertions = 0;
		var tests = {};
		var test_names = [];
		var executing_tests = [];
	
		var app_dir = TFS.getApplicationDirectory();
		var test_harness_dir = TFS.getFile(TA.appURLToPath('app://test_harness'));
		var results_dir = TFS.getFile(TA.appURLToPath('app://test_results'));
		var drillbit_funcs = TFS.getFile(TA.appURLToPath('app://drillbit_func.js')).read();
		var user_scripts_dir = null;
		var app = null;
		var tiapp_backup = null, tiapp = null;
		var manifest_backup = null, manifest = null;
		var non_visual_ti = null;
		var tests_started = 0;
		var self = this;
		
		this.include = function(path)
		{
			var code = TFS.getFile(path).read();
			Titanium.App.stdout(code.toString());
			
			try {
				with (this) {
					eval(code.toString());
				}
			} catch (e) {
				Titanium.App.stdout("Error: "+String(e)+", "+path+", line:"+e.line);
			}
		};
		
		function describe(description,test)
		{
			current_test_load.description = description;
			current_test_load.test = test;
			current_test_load.timeout = test.timeout || 5000;
			current_test_load.assertions = {};
			current_test_load.assertion_count = 0;

			for (var p in test)
			{
				if (excludes.indexOf(p)==-1)
				{
					var fn = test[p];
					if (typeof fn == 'function')
					{
						total_tests++;
						current_test_load.assertion_count++;
						current_test_load.assertions[p]=false;
					}
				}
			}

			total_files++;
			current_test_load = null;
		};
	
		/*this.make_function = function(f,scope)
		{
			if (typeof(f)=='function')
			{
				if (typeof(scope)=='undefined')
				{
					return '(' + String(f) + ')();\n';
				}
				else
				{
					var expr = '(function(){var _scope = ' + scope + ';\n';
					expr+='(' + String(f) + ').call(_scope,_scope);\n';
					expr+='})();\n';
					return expr;
				}
			}
			return '';
		};*/
	
		this.loadTests = function(test_files)
		{

			results_dir.createDirectory();

			var f = Titanium.Filesystem.getFile(results_dir, "results.html");
			if (f.exists()) {
				f.deleteFile();
			}
		
			for (var c=0;c<test_files.length;c++)
			{
				var f = test_files[c];
				var name = f.name();
				var ext = f.extension();
				name = name.replace('.'+ext,'');
				var dir = f.parent();
				var jsfile = TFS.getFile(dir,name+'.js');
				if (!jsfile.exists())
				{
					continue;
				}
				var entry = tests[name];
				if (!entry)
				{
					entry = {name:name,dir:dir};
					tests[name] = entry;
					test_names.push(name);
				}
				entry[ext] = f;
				current_test_load = entry;
				try
				{
					eval(String(jsfile.read()));
				}
				catch(EX)
				{
					frontend.error("error loading: "+f+". Exception: "+EX+" (line: "+EX.line+")");
					//Titanium.API.debug("error loading: "+f+". Exception: "+EX+" (line: "+EX.line+")");
				}
			}

			test_names.sort();
		};
	
		this.setupTestHarness = function(harness_manifest)
		{
			var runtime_dir = TFS.getFile(Titanium.Process.getCurrentProcess().getEnvironment('KR_RUNTIME'));
			var modules_dir = TFS.getFile(TFS.getApplicationDirectory(),'modules');

			// create the test harness directory
			if (!test_harness_dir.exists())
			{
				test_harness_dir.createDirectory();
			}

			// create app structure
			app = Titanium.createApp(runtime_dir,test_harness_dir,'test_harness','CF0D2CB7-B4BD-488F-9F8E-669E6B53E0C4',false);
			tiapp_backup = TFS.getFile(app.base,'_tiapp.xml');
			manifest_backup = TFS.getFile(app.base,'_manifest');

			//var mymanifest = TFS.getFile(TFS.getApplicationDirectory(),'manifest');
			manifest = TFS.getFile(app.base,'manifest');
			var manifest_contents = harness_manifest.read().toString();

			manifest_contents = manifest_contents.replace('UnitTest','UnitTest Harness');
			manifest_contents = manifest_contents.replace('com.titaniumapp.unittest.driver','com.titaniumapp.unittest');
			manifest_contents = manifest_contents.replace('D83B08F4-B43B-4909-9FEE-336CDB44750B','CF0D2CB7-B4BD-488F-9F8E-669E6B53E0C4');
			manifest_contents = manifest_contents.replace('Unit Test Driver','Unit Test Harness');
			manifest.write(manifest_contents);

			tiapp = Titanium.Project.writeTiXML('com.titaniumapp.unittest','test_harness','Appcelerator','http://titaniumapp.com',app.base);
			tiapp.copy(tiapp_backup);
			manifest.copy(manifest_backup);

			var ti_contents = String(tiapp.read());
			non_visual_ti = ti_contents.replace('<visible>true</visible>','<visible>false</visible>');

			// copy in our user script which is the driver
			user_scripts_dir = TFS.getFile(app.resources,'userscripts');
			user_scripts_dir.createDirectory();
		};
	
		this.runTests = function()
		{	
			for (var i = 0; i < test_names.length; i++)
			{
				var name = test_names[i];
				var entry = tests[name];
				executing_tests.push(entry);
				running_tests+=entry.assertion_count;
			}
		
			tests_started = new Date().getTime();
			this.run_next_test();
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

			/*
			var us = '// ==UserScript==\n';
			us+='// @name	Titanium App Tester\n';
			us+='// @author	Appcelerator\n';
			us+='// @description	Titanium Tests\n';
			us+='// @include	app://com.titaniumapp.unittest/index.html\n';
			us+='// @version 	0.1\n';
			us+='// ==/UserScript==\n\n';

			us+=drillbit_funcs + '\n';
			us+="TitaniumTest.NAME = '"+entry.name+"';\n";

			us+="try{";
			us+=this.make_function(entry.test.before_all,'TitaniumTest.gscope');
			us+="}catch(e){Titanium.API.error('before_all caught error:'+e+' at line: '+e.line);}\n";

			// we skip these from being re-included
			for (var f in entry.test)
			{
				var i = excludes.indexOf(f);
				if (i==-1)
				{
					us+="TitaniumTest.tests.push(function(){\n";
					us+="// "+f+"\n";
					us+="var xscope = new TitaniumTest.Scope('"+f+"');"
					us+=this.make_function(entry.test.before,'xscope');

					us+="try {\n";
					us+="TitaniumTest.currentTest='" + f + "';\n";
					us+="TitaniumTest.runningTest('"+entry.name+"','" + f + "');\n";
					us+=this.make_function(entry.test[f],'xscope');

					i = f.indexOf('_as_async');
					if (i==-1)
					{
						us+="TitaniumTest.testPassed('"+f+"',TitaniumTest.currentSubject.lineNumber);\n";
					}

					us+="}\n";
					us+="catch(___e){\n";
					us+="TitaniumTest.testFailed('"+f+"',___e);\n";
					us+="}";

					us+=this.make_function(entry.test.after,'xscope');
					us+="//--- "+f+" ---\n";
					us+="});\n\n"
				}
			}

			us+="TitaniumTest.on_complete = function(){\n";
			us+="try{";
			us+=this.make_function(entry.test.after_all,'TitaniumTest.gscope');
			us+="}catch(e){Titanium.API.error('after_all caught error:'+e+' at line: '+e.line);}\n";
			us+="TitaniumTest.complete();\n";
			us+="};\n";
			us+="TitaniumTest.run_next_test();\n";

			// poor man's hack to insert line numbers
			var newus = '';
			var lines = us.split("\n");
			var ready = false;
			for (var linenum=0;linenum<lines.length;linenum++)
			{
				var line = lines[linenum];
				if (!ready)
				{
					if (line.indexOf('TitaniumTest.NAME')==0)
					{
						ready = true;
					}
					newus+=line+"\n";
					continue;
				}
				var idx = line.indexOf('should_');
				if (idx != -1)
				{
					var endIdx = line.lastIndexOf(')');
					if (line.charAt(endIdx-1)=='(')
					{
						line = line.substring(0,endIdx) + 'null,' + (linenum+1) + ');';
					}
					else
					{
						line = line.substring(0,endIdx) + ',' + (linenum+1) + ');';
					}
				}
				newus+=line+"\n";
			}*/
			
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
				Titanium.App.stdout("error: "+e+",line:"+e.line);
			}
			TFS.getFile(module.getPath(),"template_out.js").write(user_script);
			runner_js.write(user_script);

			var profile_path = TFS.getFile(results_dir,entry.name+'.prof');
			var log_path = TFS.getFile(results_dir,entry.name+'.log');

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

			args.push('--results-dir="' + results_dir + '"');
			var process = Titanium.Process.createProcess(args);
			var passed = 0;
			var failed = 0;
			process.setOnRead(function(event)
			{
				for (var data = event.pipe.readLine(); data != null; data = event.pipe.readLine())
				{
					var i = data.indexOf('DRILLBIT_');
					if (i != -1)
					{
						if (data.indexOf('DRILLBIT_TEST:') != -1) {
							var comma = data.indexOf(',');
							var suite_name = data.substring(15,comma);
							var test_name = data.substring(comma+1,data.length-1);

							//show_current_test(suite_name,test_name);
							continue;
						}
						else if (data.indexOf('DRILLBIT_ASSERTION:') != -1) {
							var comma = data.indexOf(',');
							var test_name = data.substring('DRILLBIT_ASSERTION:'.length+1, comma);
							var line_number = data.substring(comma+1,data.length-1);
							// do something with metadata eventually?
							//add_assertion();
							continue;
						}

						var test_name = data.substring(15);
						var test_passed = data.indexOf('_PASS:')!=-1;
						running_completed++;
						if (test_passed) { passed++; running_passed++; }
						else { failed ++; running_failed++; }

						//suite_progress(passed, failed, current_test.assertion_count);
						//total_progress(running_passed, running_failed, total_tests);

						var msg = "Completed: " +entry.name + " ... " + running_completed + "/" + running_tests;
						//update_status(msg);
						Titanium.API.info("test ["+ test_name + "], passed:"+test_passed);
					}
					else
					{
						Titanium.API.debug("PROCESS:"+data);
					}
				}
			});
			var size = 0;
			var timer = null;
			var start_time = new Date().getTime();
			var original_time = start_time;

			// start a stuck process monitor in which we check the 
			// size of the profile file -- if we're not doing anything
			// we should have a file that hasn't changed in sometime
			/*timer = setInterval(function()
			{
				var t = new Date().getTime();
				var newsize = profile_path.size();
				var timed_out = (t-original_time) > 40000;
				if (newsize == size || timed_out)
				{
					if (timed_out || t-start_time>=entry.timeout)
					{
						clearInterval(timer);
						current_test.failed = true;
						update_status(current_test.name + " timed out");
						test_status(current_test.name,'Failed');
						process.terminate();
						return;
					}
				}
				else
				{
					size = newsize;
				}
				start_time = t;
			},1000);*/

			process.setOnExit(function(exitcode)
			{
				Titanium.API.debug("test has exited: "+current_test.name);
				try
				{
					//clearInterval(timer);
					if (!current_test.failed)
					{
						var r = TFS.getFile(results_dir,current_test.name+'.json').read();
						var rs = '(' + r + ');';
						var results = eval(rs);
						current_test.results = results;
						//test_status(current_test.name,results.failed>0?'Failed':'Passed');
						//update_status(current_test.name + ' complete ... '+results.passed+' passed, '+results.failed+' failed');
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
					Titanium.API.error("onexit failure = "+E+" at "+E.line);
				}
				this.run_next_test();
			});
			process.launch();
		};
	
		this.run_next_test = function()
		{
			if (executing_tests==null || executing_tests.length == 0)
			{
				var test_duration = (new Date().getTime() - tests_started)/1000;
				executing_tests = null;
				current_test = null;
				run_button.disabled = false;
				//update_status('Testing complete ... took ' + test_duration + ' seconds',true);
				var f = TFS.getFile(results_dir,'drillbit.json');
				f.write("{\"success\":" + String(!test_failures) + "}");
				if (this.auto_close)
				{
					Titanium.App.exit(test_failures ? 1 : 0);
				}
				return;
			}
			var entry = executing_tests.shift();
			current_test = entry;
			current_test.failed = false;
//			update_status('Executing: '+entry.name+' ... '+running_completed + "/" + running_tests);
//			test_status(entry.name,'Running');
			//setTimeout(function(){this.run_test(entry)},1);
			this.run_test(entry);
		};
	};
	
	Titanium.Drillbit = new Drillbit();
})();