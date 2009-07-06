
var TFS = Titanium.Filesystem;
var TA  = Titanium.App;

var current_test_load = null;
var current_test = null;
var excludes = ['before','before_all','after','after_all','timeout'];
var total_tests = 0;
var total_files = 0;
var running_tests = 0;
var running_completed = 0;
var running_passed = 0;
var running_failed = 0;
var auto_run = false;
var auto_close = false;
var debug_tests = false;
var test_failures = false;
var specific_tests = null;
var total_assertions = 0;

function update_status(msg,hide)
{
	$('#status').html(msg).css('visibility','visible');
	if (typeof(hide)!='undefined')
	{
		setTimeout(function()
		{
			$('#status').css('visibility','hidden');
		},4000);
	}
}
function test_status(name,classname)
{
	var el = $('#test_'+name+' td.status');
	el.html(classname);
	el.removeClass('untested').removeClass('failed').removeClass('running')
	  .removeClass('passed').addClass(classname.toLowerCase());
}

function progress(passed,failed,total,progress)
{
	var passed_percent = Math.floor((passed/total) * 100);
	var failed_percent = Math.floor((failed/total) * 100);
	progress.html(
		'<div class="passed-progress" style="width: '+passed_percent+'%;">&nbsp;</div>'+
		'<div class="failed-progress" style="width: '+failed_percent+'%;">&nbsp;</div>'
	);
}

function suite_progress(passed,failed,total)
{
	progress(passed,failed,total,$('#suite-progress'));
}

function total_progress(passed,failed,total)
{
	progress(passed,failed,total,$('#total-progress'));
	
	var passed_percent = Math.floor((passed/total) * 100);
	var failed_percent = Math.floor((failed/total) * 100);
	$('#progress-message').html('<img src="images/check_on.png"/>'+passed+'&nbsp;&nbsp; <img src="images/check_off.png"/>'+failed);
}

function show_current_test(suite_name, test_name)
{
	$('#current-test-message').text(suite_name + ': ' + test_name);
}

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
	
	$('#header').html(total_tests+' tests in '+total_files+' files');

	current_test_load = null;
}

function add_assertion()
{
	total_assertions++;
	$('#header').html(total_tests+' tests in '+total_files+' files<br/>'+total_assertions+' assertions');
}

function make_function(f,scope)
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
}

function show_test_details(name)
{
	var w = Titanium.UI.currentWindow.createWindow();
	w.setHeight(600);
	w.setWidth(600);
	w.setURL('app://test_results/' + name + '.html');
	w.open();
}

function toggle_test_includes()
{
	$.each($('#table .checkbox'),function()
	{
		if ($(this).is('.checked'))
		{
			$(this).removeClass('checked');
		}
		else
		{
			$(this).addClass('checked');
		}
	});
}

function select_tests(tests)
{
	// clear the table
	$.each($('#table .checkbox'),function()
	{
		if ($(this).is('.checked')) {
			$(this).removeClass('checked');
		}
	});
	
	//select tests
	for (var t=0; t < tests.length; t++) {
		var test = tests[t];
		
		$('#test_'+test+' .checkbox').addClass('checked');
	}
}

var tests = {};
window.onload = function()
{
	var test_names = [];
	
	var test_dir = TFS.getFile(TA.appURLToPath('app://tests'));
	var test_harness_dir = TFS.getFile(TA.appURLToPath('app://test_harness'));
	var results_dir = TFS.getFile(TA.appURLToPath('app://test_results'));
	var drillbit_funcs = TFS.getFile(TA.appURLToPath('app://drillbit_func.js')).read();
	var dir_list = test_dir.getDirectoryListing();
	
	results_dir.createDirectory();
	
	var f = Titanium.Filesystem.getFile(results_dir, "results.html");
	if (f.exists()) {
		f.deleteFile();
	}
	
	for (var c=0;c<dir_list.length;c++)
	{
		var f = dir_list[c];
		if (!f.isDirectory())
		{
			continue;
		}
		var name = f.name();
		var ext = f.extension();
		name = name.replace('.'+ext,'');
		var dir = TFS.getFile(test_dir,name);
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
			alert("error loading: "+f+". Exception: "+EX+" (line: "+EX.line+")");
			Titanium.API.debug("error loading: "+f+". Exception: "+EX+" (line: "+EX.line+")");
		}
	}

	test_names.sort();
	
	
	var table = '<table>' +
		'<tr>'+
			'<th width="25" onclick="toggle_test_includes();" style="cursor:pointer">Include</th>'+
			'<th>Test</th>'+
			'<th>Description</th>'+
			'<th width="25">Result</th>'+
		'</tr>'+
	'';
		
	for (var c=0;c<test_names.length;c++)
	{
		var name = test_names[c];
		var entry = tests[name];
		
		table+=
		'<tr id="test_'+name+'" class="test">'+
			'<td class="check"><div class="checkbox checked"></div></td>'+
			'<td class="name">'+name+'</td>'+
			'<td class="description">'+entry.description+'</td>'+
			'<td class="status untested" onclick="show_test_details(\'' + name + '\')">Untested</td>'+
		'</tr>';
	}
	
	table+='</table>';
	
	$('#table').html(table);
	$('#table .checkbox').click(function()
	{
		if ($(this).is('.checked'))
		{
			$(this).removeClass('checked');
		}
		else
		{
			$(this).addClass('checked');
		}
	});
		
	// get the runtime dir
	var runtime_dir = TFS.getFile(Titanium.Process.getEnv('KR_RUNTIME'));
	var modules_dir = TFS.getFile(TFS.getApplicationDirectory(),'modules');
	var app_dir = TFS.getApplicationDirectory();
	
	var run_button = $('#run').get(0);

	// create the test harness directory
	if (!test_harness_dir.exists())
	{
		test_harness_dir.createDirectory();
	}
	
	// create app structure
	var app = Titanium.createApp(runtime_dir,test_harness_dir,'test_harness','CF0D2CB7-B4BD-488F-9F8E-669E6B53E0C4',false);
	
	var executing_tests = null;
	var tiapp_backup = TFS.getFile(app.base,'_tiapp.xml');
	var manifest_backup = TFS.getFile(app.base,'_manifest');
	
	var mymanifest = TFS.getFile(TFS.getApplicationDirectory(),'manifest');
	var manifest = TFS.getFile(app.base,'manifest');
	var manifest_contents = mymanifest.read().toString();

	manifest_contents = manifest_contents.replace('UnitTest','UnitTest Harness');
	manifest_contents = manifest_contents.replace('com.titaniumapp.unittest.driver','com.titaniumapp.unittest');
	manifest_contents = manifest_contents.replace('D83B08F4-B43B-4909-9FEE-336CDB44750B','CF0D2CB7-B4BD-488F-9F8E-669E6B53E0C4');
	manifest_contents = manifest_contents.replace('Unit Test Driver','Unit Test Harness');

	manifest.write(manifest_contents);
	
	var tiapp = Titanium.Project.writeTiXML('com.titaniumapp.unittest','test_harness','Appcelerator','http://titaniumapp.com',app.base);
	
	tiapp.copy(tiapp_backup);
	manifest.copy(manifest_backup);

	var ti_contents = String(tiapp.read());
	var non_visual_ti = ti_contents.replace('<visible>true</visible>','<visible>false</visible>');
	
	// copy in our user script which is the driver
	var user_scripts_dir = TFS.getFile(app.resources,'userscripts');
	user_scripts_dir.createDirectory();
	
	var tests_started = 0;
	
	function run_test(entry)
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
		us+=make_function(entry.test.before_all,'TitaniumTest.gscope');
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
				us+=make_function(entry.test.before,'xscope');
				
				us+="try {\n";
				us+="TitaniumTest.currentTest='" + f + "';\n";
				us+="TitaniumTest.runningTest('"+entry.name+"','" + f + "');\n";
				us+=make_function(entry.test[f],'xscope');
				
				i = f.indexOf('_as_async');
				if (i==-1)
				{
					us+="TitaniumTest.testPassed('"+f+"',TitaniumTest.currentSubject.lineNumber);\n";
				}

				us+="}\n";
				us+="catch(___e){\n";
				us+="TitaniumTest.testFailed('"+f+"',___e);\n";
				us+="}";

				us+=make_function(entry.test.after,'xscope');
				us+="//--- "+f+" ---\n";
				us+="});\n\n"
			}
		}
		
		us+="TitaniumTest.on_complete = function(){\n";
		us+="try{";
		us+=make_function(entry.test.after_all,'TitaniumTest.gscope');
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
		}
		
		var runner_js = TFS.getFile(user_scripts_dir,entry.name+'_driver.js');
		runner_js.write(newus);
		
		
		var profile_path = TFS.getFile(results_dir,entry.name+'.prof');
		var log_path = TFS.getFile(results_dir,entry.name+'.log');

		profile_path.deleteFile();
		log_path.deleteFile();

		var args = ['--profile="'+profile_path+'"']
		args.push('--logpath="'+log_path+'"')
		args.push('--bundled-component-override="'+app_dir+'"')
		args.push('--no-console-logging');
		args.push('--debug');
		if (debug_tests) {
			args.push('--attach-debugger');
		}
		
		args.push('--results-dir="' + results_dir + '"');
		var process = Titanium.Process.launch(app.executable.nativePath(),args);
		var passed = 0;
		var failed = 0;
		process.onread = function(d)
		{
			var lines = d.split('\n');
			for (var l = 0; l < lines.length; l++)
			{
				var data = lines[l];
				var i = data.indexOf('DRILLBIT_');
				if (i != -1)
				{
					if (data.indexOf('DRILLBIT_TEST:') != -1) {
						var comma = data.indexOf(',');
						var suite_name = data.substring(15,comma);
						var test_name = data.substring(comma+1,data.length-1);
					
						show_current_test(suite_name,test_name);
						continue;
					}
					else if (data.indexOf('DRILLBIT_ASSERTION:') != -1) {
						var comma = data.indexOf(',');
						var test_name = data.substring('DRILLBIT_ASSERTION:'.length+1, comma);
						var line_number = data.substring(comma+1,data.length-1);
						// do something with metadata eventually?
						add_assertion();
						continue;
					}
						
					var test_name = data.substring(15);
					var test_passed = data.indexOf('_PASS:')!=-1;
					running_completed++;
					if (test_passed) { passed++; running_passed++; }
					else { failed ++; running_failed++; }
					
					suite_progress(passed, failed, current_test.assertion_count);
					total_progress(running_passed, running_failed, total_tests);
					
					var msg = "Completed: " +entry.name + " ... " + running_completed + "/" + running_tests;
					update_status(msg);
					Titanium.API.debug("test ["+ test_name + "], passed:"+test_passed);
				}
				else
				{
					Titanium.API.debug("PROCESS:"+data);
				}
			}
		};
		var size = 0;
		var timer = null;
		var start_time = new Date().getTime();
		var original_time = start_time;
		
		// start a stuck process monitor in which we check the 
		// size of the profile file -- if we're not doing anything
		// we should have a file that hasn't changed in sometime
		timer = setInterval(function()
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
		},1000);
		
		process.onexit = function(exitcode)
		{
			Titanium.API.debug("test has exited: "+current_test.name);
			try
			{
				clearInterval(timer);
				if (!current_test.failed)
				{
					var r = TFS.getFile(results_dir,current_test.name+'.json').read();
					var rs = '(' + r + ');';
					var results = eval(rs);
					current_test.results = results;
					test_status(current_test.name,results.failed>0?'Failed':'Passed');
					update_status(current_test.name + ' complete ... '+results.passed+' passed, '+results.failed+' failed');
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
			run_next_test();
		};
	}
	
	function run_next_test()
	{
		if (executing_tests==null || executing_tests.length == 0)
		{
			var test_duration = (new Date().getTime() - tests_started)/1000;
			executing_tests = null;
			current_test = null;
			run_button.disabled = false;
			update_status('Testing complete ... took ' + test_duration + ' seconds',true);
			var f = TFS.getFile(results_dir,'drillbit.json');
			f.write("{\"success\":" + String(!test_failures) + "}");
			if (auto_close)
			{
				Titanium.App.exit(test_failures ? 1 : 0);
			}
			return;
		}
		var entry = executing_tests.shift();
		current_test = entry;
		current_test.failed = false;
		update_status('Executing: '+entry.name+' ... '+running_completed + "/" + running_tests);
		test_status(entry.name,'Running');
		setTimeout(function(){run_test(entry)},1);
	}
	
	run_button.onclick = function ()
	{
		run_button.disabled = true;
		update_status('Building test harness ... one moment');
		executing_tests = [];
		running_tests = 0;
		running_completed = 0;
		
		$.each($('#table tr.test'),function()
		{
			var name = $(this).find('.name').html();
			var entry = tests[name];
			test_status(entry.name,'Untested');
			if ($(this).find('.checkbox').is('.checked'))
			{
				executing_tests.push(entry);
				running_tests+=entry.assertion_count;
			}
		});
		
		tests_started = new Date().getTime();
		setTimeout(run_next_test,1);
	};
	
	// if you pass in --autorun, just go ahead and start
	for (var c=0;c<Titanium.App.arguments.length;c++)
	{
		var arg = Titanium.App.arguments[c];
		
		if (arg == '--autorun')
		{
			auto_run = true;
			run_button.click();
			//break;
		}
		else if (arg == '--autoclose')
		{
			auto_close = true;
		}
		else if (arg == '--debug-tests')
		{
			debug_tests = true;
		}
		else if (arg.indexOf('--tests=')==0)
		{
			specific_tests = arg.substring(8).split(',');
			select_tests(specific_tests);
		}
	}
};

