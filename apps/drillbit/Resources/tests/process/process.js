describe("process tests",
{
	before_all: function()
	{
		this.dirCmd = Titanium.platform == "win32" ? ["C:\\Windows\\System32\\cmd.exe","/C","dir"] : ["/bin/ls"];
		this.echoCmd = Titanium.platform == "win32" ? ["C:\\Windows\\System32\\cmd.exe", "/C", "echo"] : ["/bin/echo"];
		this.moreCmd = Titanium.platform == "win32" ? ["C:\\Windows\\System32\\more.com"]: ["/usr/bin/more"];
		
		this.cmd = function(args, args2) {
			Array.prototype.push.apply(args, args2);
		}
	},
	
	test_process_object: function()
	{
		value_of(Titanium.Process).should_not_be_null();
		value_of(Titanium.Process.createProcess).should_be_function();
		value_of(Titanium.Process.createInputPipe).should_be_function();
		value_of(Titanium.Process.createOutputPipe).should_be_function();
		value_of(Titanium.Process.getCurrentProcess).should_be_function();
	},
	
	test_input_pipe: function()
	{
		var i = Titanium.Process.createInputPipe();
		value_of(i).should_be_object();
		value_of(i.read).should_be_function();
		value_of(i.isSplit).should_be_function();
		value_of(i.split).should_be_function();
		value_of(i.unsplit).should_be_function();
		value_of(i.attach).should_be_function();
		value_of(i.detach).should_be_function();
		value_of(i.isAttached).should_be_function();
		value_of(i.join).should_be_function();
		value_of(i.unjoin).should_be_function();
		value_of(i.isJoined).should_be_function();
		value_of(i.isClosed).should_be_function();
		value_of(i.close).should_be_function();
		
		value_of(i.isClosed()).should_be_false();
		i.close();
		value_of(i.isClosed()).should_be_true();
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
	
	test_process_on_PATH: function()
	{
		var dir = Titanium.platform == "win32" ? ["cmd", "/C", "dir"] : ["ls"];
		var listDirs = Titanium.Process.createProcess(dir);
		//value_of(listDirs.getArguments()[0]).should_be(this.dirCmd);
		var dirList = listDirs();
		value_of(dirList.length).should_be_greater_than(0);
	},
	
	test_split_and_attach_pipe_as_async: function(callback)
	{
		var originalData = 'this_is_a_split_and_attach_test';
		var echoCmd = this.echoCmd.slice();
		echoCmd.push(originalData);
		
		var p = Titanium.Process.createProcess(echoCmd);
		p.stdout.join(p.stderr);
		value_of(p.stdout.isJoined()).should_be_false();
		value_of(p.stderr.isJoined()).should_be_true();
		
		var pipes = p.stdout.split();
		value_of(pipes.length).should_be(2);
		value_of(pipes[0]).should_be_object();
		value_of(pipes[1]).should_be_object();
		value_of(p.stdout.isSplit()).should_be_true();
		
		var file = Titanium.Filesystem.createTempFile();
		var stream = Titanium.Filesystem.getFileStream(file.nativePath());
		stream.open(stream.MODE_WRITE);
		
		pipes[0].attach(stream);
		value_of(pipes[0].isAttached()).should_be_true();
		value_of(pipes[1].isAttached()).should_be_false();
		value_of(p.stdout.isAttached()).should_be_false();
		
		var data = "";
		pipes[1].setOnRead(function(event){
			data += event.pipe.read();
		});
		var timer = 0;
		p.setOnExit(function(exitCode){
			clearTimeout(timer);
			stream.close();
			var fileData = file.read();
			
			try
			{
				value_of(data).should_be(fileData);
				value_of(data).should_be(originalData);
				value_of(pipes[0].isClosed()).should_be_true();
				value_of(pipes[1].isClosed()).should_be_true();
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
		Titanium.API.debug(echoCmd+","+this.moreCmd);
		
		var echo = Titanium.Process.createProcess(echoCmd);
		var more = Titanium.Process.createProcess(this.moreCmd);
		echo.stdout.attach(more.stdin);
		value_of(echo.stdout.isAttached()).should_be_true();
		var moreData = "";
		more.setOnRead(function(event){
			moreData += event.pipe.read();
		});
		var timer = 0;
		more.setOnExit(function(exitCode){
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
			callback.failed("Timed out waiting for command to exit");
		}, 5000);
	},
	
	test_input_pipe_empty_read: function() {
		var i = Titanium.Process.createInputPipe();
		var exception = false;
		var data = null;
		try { data = i.read(); } catch(e) { exception = true; }
		
		value_of(exception).should_be_false();
		value_of(data).should_be_object();
		value_of(data.length).should_be(0);	
	},
	
	test_output_pipe: function()
	{
		var o = Titanium.Process.createOutputPipe();
		value_of(o).should_be_object();
		value_of(o.write).should_be_function();
		value_of(o.isClosed).should_be_function();
		value_of(o.close).should_be_function();
		
		value_of(o.isClosed()).should_be_false();
		o.close();
		value_of(o.isClosed()).should_be_true();
	},
	
	test_output_pipe_write: function()
	{
		var o = Titanium.Process.createOutputPipe();
		var blob = Titanium.CoreTypes.createBlob("some data");
		var written = o.write(blob);
		value_of(written).should_be(blob.length);
		o.close();
	},
	
	test_current_process: function()
	{
		var p = Titanium.Process.getCurrentProcess();
		value_of(p).should_not_be_null();
		value_of(p.getEnvironment).should_be_function();
		value_of(p.setEnvironment).should_be_function();
		value_of(p.cloneEnvironment).should_be_function();
		value_of(p.getArguments).should_be_function();
		value_of(p.stdin).should_be_null();
		value_of(p.stdout).should_be_null();
		value_of(p.stderr).should_be_null();
	},
	
	test_current_process_env: function()
	{
		var p = Titanium.Process.getCurrentProcess();
		value_of(p.getEnvironment("foobar")).should_be_undefined();
		p.setEnvironment("foobar", "1");
		value_of(p.getEnvironment("foobar")).should_be("1");
		var e = p.cloneEnvironment();
		value_of(e["foobar"]).should_be("1");
	},
	
	test_clone_environment: function()
	{
		var ce = Titanium.Process.getCurrentProcess().cloneEnvironment();
		var e = Titanium.Process.getCurrentProcess().getEnvironment();
		
		for (name in e) {
			value_of(name in ce).should_be_true();
			value_of(ce[name]).should_be(e[name]);
		}
	},
	
	test_current_process_arguments: function()
	{
		var p = Titanium.Process.getCurrentProcess();
		
		value_of(p.getArguments().length).should_be(Titanium.App.arguments.length+1);
		for (var i = 1; i < p.getArguments().length; i++)
		{
			value_of(Titanium.App.arguments).should_contain(p.getArguments()[i]);
		}
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
			var buf = event.pipe.read();
			output += buf;
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
	}
});
