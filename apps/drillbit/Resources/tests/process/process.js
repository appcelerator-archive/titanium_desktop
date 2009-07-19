describe("process tests",
{
	before_all: function()
	{
		this.dirCmd = Titanium.platform == "win32" ? ["C:\\Windows\\System32\\cmd.exe","/C","dir"] : ["/bin/ls"];
		this.echoCmd = Titanium.platform == "win32" ? ["C:\\Windows\\System32\\cmd.exe", "/C", "echo"] : ["/bin/echo"];
		this.moreCmd = Titanium.platform == "win32" ? ["C:\\Windows\\System32\\more.com"]: ["/usr/bin/more"];
	},
	
	test_process_binding: function()
	{
		value_of(Titanium.Process).should_not_be_null();
		value_of(Titanium.Process.createProcess).should_be_function();
		value_of(Titanium.Process.createPipe).should_be_function();
	},
	
	test_process_object: function()
	{
		var p = Titanium.Process.createProcess(this.dirCmd);
		value_of(p).should_be_function();
		value_of(p.getStdin).should_be_function();
		value_of(p.getStdout).should_be_function();
		value_of(p.getStderr).should_be_function();
		value_of(p.getStdin()).should_be_object();
		value_of(p.getStdout()).should_be_object();
		value_of(p.getStderr()).should_be_object();
		value_of(p.getPID).should_be_function();
		value_of(p.getExitCode).should_be_function();
		value_of(p.getArguments).should_be_function();
		value_of(p.getArguments()).should_be_array(this.dirCmd);
		value_of(p.getEnvironment).should_be_function();
		value_of(p.setEnvironment).should_be_function();
		value_of(p.cloneEnvironment).should_be_function();
		value_of(p.launch).should_be_function();
		value_of(p.kill).should_be_function();
		value_of(p.terminate).should_be_function();
		value_of(p.sendSignal).should_be_function();
		value_of(p.setOnRead).should_be_function();
		value_of(p.setOnExit).should_be_function();
		value_of(p.isRunning).should_be_function();
		value_of(p.isRunning()).should_be_false();
	},
	
	test_create_process: function()
	{
		value_of(function(){ Titanium.Process.createProcess(); }).should_throw_exception();
		value_of(function(){ Titanium.Process.createProcess(null); }).should_throw_exception();
		value_of(function(){ Titanium.Process.createProcess([null]); }).should_throw_exception();
		value_of(function(){ Titanium.Process.createProcess([]); }).should_throw_exception();
		
		var p = Titanium.Process.createProcess(['ls',true]);
		value_of(p.getArguments()).should_be_array(['ls', 'true']);
		
		p = Titanium.Process.createProcess(['ls',['a','b']]);
		value_of(p.getArguments()).should_be_array(['ls', 'a', 'b']);
	},
	
	test_named_args: function()
	{
		var ins = Titanium.Process.createPipe();
		var outs = Titanium.Process.createPipe();
		var errs = Titanium.Process.createPipe();
		 
		var myProgram = Titanium.Process.createProcess({  
		  args: ['myprogram'],  
		  env: {'env_var_1': 'value_1'},
		  stdin: ins,
		  stdout: outs,
		  stderr: errs
		});
		
		value_of(myProgram.getArguments()).should_be_array(['myprogram']);
		value_of(myProgram.getEnvironment()['env_var_1']).should_be('value_1');
		value_of('PATH' in myProgram.getEnvironment()).should_be_false();
	},
	
	test_pipe: function()
	{
		var i = Titanium.Process.createPipe();
		value_of(i).should_be_object();
		value_of(i.attach).should_be_function();
		value_of(i.detach).should_be_function();
		value_of(i.isAttached).should_be_function();
		value_of(i.close).should_be_function();
	},
	
	test_anonymous_attach_as_async: function(callback)
	{
		var p = Titanium.Process.createProcess(this.dirCmd);
		var buf = "";
		p.stdout.attach({
			write: function(data) {
				buf += data.toString();
			}
		});
		var timer = 0;
		p.setOnExit(function(event){
			clearTimeout(timer);
			try {
				value_of(buf.length).should_be_greater_than(0);
			} catch (e) {
				callback.failed(e);
			}
			callback.passed();
		});
		p.launch();
		
		timer = setTimeout(function(){
			callback.failed("timed out");
		}, 3000);
	},
	
	test_kill_as_async: function(callback)
	{
		var p = Titanium.Process.createProcess(this.moreCmd);
		var timer = 0;
		p.setOnExit(function(){
			clearTimeout(timer);
			callback.passed();
		});
		p.launch();
		setTimeout(function(){
			p.kill();
		}, 500);
		
		timer = setTimeout(function(){
			callback.failed("timed out");
		}, 3000);
	},
	
	test_terminate_as_async: function(callback)
	{
		var p = Titanium.Process.createProcess(this.moreCmd);
		var timer = 0;
		p.setOnExit(function(){
			clearTimeout(timer);
			callback.passed();
		});
		p.launch();
		setTimeout(function(){
			p.terminate();
		}, 500);
		
		timer = setTimeout(function(){
			callback.failed("timed out");
		}, 3000);
	},
	
	test_sync_process: function()
	{
		var p = Titanium.Process.createProcess(this.dirCmd);
		value_of(p).should_be_function();
		
		var list = p();
		value_of(list).should_be_object();
		value_of(list.toString).should_be_function();
		
		var data = list.toString();
		value_of(data).should_be_string();
		value_of(data.length).should_be_greater_than(0);
		
		value_of(p.stdin).should_be_object();
		value_of(p.stdout).should_be_object();
		value_of(p.stderr).should_be_object();
		value_of(p.stdin.isClosed()).should_be_true();
		value_of(p.stdout.isClosed()).should_be_true();
		value_of(p.stderr.isClosed()).should_be_true();
	},
	
	test_attach_file_as_async: function(callback)
	{
		var originalData = 'this_is_a_split_and_attach_test';
		var echoCmd = this.echoCmd.slice();
		echoCmd.push(originalData);
		
		var p = Titanium.Process.createProcess(echoCmd);
		var file = Titanium.Filesystem.createTempFile();
		var stream = Titanium.Filesystem.getFileStream(file.nativePath());
		stream.open(stream.MODE_WRITE);
		
		p.stdout.attach(stream);
		p.stderr.attach(stream);
		value_of(p.stdout.isAttached()).should_be_true();
		value_of(p.stderr.isAttached()).should_be_true();
		
		var data = "";
		p.setOnRead(function(event){
			data += event.data.toString();
		});
		var timer = 0;
		p.setOnExit(function(event){
			clearTimeout(timer);
			stream.close();
			var fileData = file.read();
			
			try
			{
				value_of(data).should_be(fileData);
				value_of(data.replace(/[\r\n]+/,'')).should_be(originalData);
				value_of(p.stdout.isClosed()).should_be_true();
				value_of(p.stderr.isClosed()).should_be_true();
			}
			catch (e)
			{
				callback.failed(e);
			}
			
			file.deleteFile();
			callback.passed();
		});
		p.launch();
		
		timer = setTimeout(function(){
			callback.failed("Timed out waiting for command to exit");
		}, 5000);
	},
	
	test_piped_command_as_async: function(callback)
	{
		var data = 'this_is_a_piped_test';
		var echoCmd = this.echoCmd.slice();
		echoCmd.push(data);
		
		var echo = Titanium.Process.createProcess(echoCmd);
		var more = Titanium.Process.createProcess(this.moreCmd);
		echo.stdout.attach(more.stdin);
		value_of(echo.stdout.isAttached()).should_be_true();
		var moreData = "";
		more.setOnRead(function(event){
			moreData += event.data.toString();
		});
		var timer = 0;
		more.setOnExit(function(event){
			clearTimeout(timer);
			try {
				value_of(moreData).should_be(data);
				value_of(echo.stdout.isClosed()).should_be_true();
				value_of(more.stdin.isClosed()).should_be_true();
			} catch (e) {
				callback.failed(e);
			}
			callback.passed();
		});
		
		echo.launch();
		more.launch();
		
		timer = setTimeout(function(){
			if (echo.isRunning()) echo.kill();
			if (more.isRunning()) more.kill();
			
			callback.failed("Timed out waiting for command to exit");
		}, 5000);
	},
	
	test_pipe_empty_read: function() {
		var i = Titanium.Process.createPipe();
		var exception = false;
		var data = null;
		try { data = i.read(); } catch(e) { exception = true; }
		
		value_of(exception).should_be_false();
		value_of(data).should_be_object();
		value_of(data.length).should_be(0);	
	},
		
	test_pipe_write: function()
	{
		var o = Titanium.Process.createPipe();
		var blob = Titanium.CoreTypes.createBlob("some data");
		var written = o.write(blob);
		value_of(written).should_be(blob.length);
		o.close();
	},
	
	test_environment: function()
	{
		var env = Titanium.API.getEnvironment();
		value_of(env["foobar"]).should_be_undefined();
		env["foobar"] = "1";
		value_of(env["foobar"]).should_be("1");
		
		//TODO implement env.clone()
		//var e = p.cloneEnvironment();
		//value_of(e["foobar"]).should_be("1");
	},
	
	test_process_as_async: function(test)
	{
		var p = Titanium.Process.createProcess(this.dirCmd);
		var timer = null;
		var shortTimer = null;
		value_of(p).should_not_be_null();
		value_of(p.stdin).should_be_object();
		value_of(p.stdout).should_be_object();
		value_of(p.stderr).should_be_object();
		
		var output = '';
		
		p.setOnRead(function(event)
		{
			try {
				value_of(p.getPID()).should_be_number();
				var buf = event.data.toString();
				value_of(buf).should_be_object();
				value_of(buf.toString()).should_be_string();
				
				output += buf.toString();
			} catch(e) {
				test.failed(e);
			}
		});
		
		p.setOnExit(function()
		{
			clearTimeout(timer);
			if (output.length > 0)
			{
				test.passed();
			}
			else
			{
				test.failed('no output received');
			}
		});
		
		p.launch();
		
		// if we hit this timeout, then we fail.		
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	},
	
	test_process_exception_as_async: function(test)
	{
		var p = Titanium.Process.createProcess(this.dirCmd);
		var timer = null;
		value_of(p).should_not_be_null();
		var output = '';
		var throwException = true;
		
		p.setOnRead(function(event)
		{
			// test throwing exception from onread
			if (throwException) {
				throw "this is an exception";
				throwException = false;
			}
		});
		p.setOnExit(function()
		{
			clearTimeout(timer);
			test.passed();
		});
		
		p.launch();
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	},
	
	test_process_exitCode_as_async: function(test)
	{
		value_of(Titanium.Process).should_not_be_null();
		var p = Titanium.Process.createProcess(this.dirCmd);
		var timer = null;
		value_of(p).should_not_be_null();
		p.setOnRead(function(event)
		{
			value_of(p.getExitCode()).should_be(-1);
		});
		p.setOnExit(function(event)
		{
			try 
			{
				var exitCode = p.getExitCode();
				value_of(exitCode).should_be_number();
				value_of(exitCode).should_be(0);
				
				test.passed();
			}
			catch(e)
			{
    			test.failed(e);
			}
			clearTimeout(timer);
		});
		
		p.launch();
		
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	},
	
	test_long_running_process_as_async: function(test)
	{
		value_of(Titanium.Process).should_not_be_null();
		var p = Titanium.Process.createProcess(Titanium.platform == "win32" ?
			['C:\\Windows\\System32\\cmd.exe','/K', 'dir'] : ['/bin/cat','-v'])
		
		var timer = null;
		var shortTimer = null;
		p.setOnRead(function(event)
		{
			// reality check... are we running?
			value_of(p.isRunning()).should_be_true();
		});
		p.setOnExit(function()
		{
			clearTimeout(timer);

			// we should not be running on exit.  
			if (this.isRunning())
			{
				test.failed('test is running onExit, shouldn\'t be');
			}
			else
			{
				test.passed();
			}
		});
		p.launch();
		
		shortTimer = setTimeout(function()
		{
			// reality check... are we running?
			value_of(p.isRunning()).should_be_true();
		
			// kill the process if it's running
			p.terminate();
		}, 1000);

		// if we hit this timeout, then we fail.
		timer = setTimeout(function()
		{
			test.failed('timed out');
		},5000);
	},
	
	test_restart_as_async: function(callback)
	{
		var p = Titanium.Process.createProcess(this.dirCmd);
		var timer = null;
		var shortTimer = null;
		var output1 = '';
		var output2 = '';
		var exited = false;
		function onRead(n)
		{
			return function(event)
			{
				try {
					value_of(p.getPID()).should_be_number();
					var buf = event.data.toString();
					value_of(buf).should_be_object();
					value_of(buf.toString()).should_be_string();
				
					n == 1 ? output1 += buf.toString() : output2 += buf.toString();
				} catch(e) {
					callback.failed(e);
				}
			};
		}
		
		p.setOnRead(onRead(1));
		p.setOnExit(function() { exited = true; });
		p.launch();
		
		shortTimer = setTimeout(function(){
			try
			{
				value_of(output1.length).should_be_greater_than(0);
				value_of(exited).should_be_true();
				
				p.setOnRead(onRead(2));
				p.setOnExit(function(event) {
					try {
						value_of(output2).should_be(output1);
						value_of(p.getExitCode()).should_be(0);
						callback.passed();
					}
					catch (e)
					{
						callback.failed(e);
					}
				});
				p.restart();
			}
			catch (e)
			{
				callback.failed(e);
			}
		}, 2000);
		
		// if we hit this timeout, then we fail.		
		timer = setTimeout(function()
		{
			test.failed('timed out waiting for process to restart');
		},5000);
	}
});
