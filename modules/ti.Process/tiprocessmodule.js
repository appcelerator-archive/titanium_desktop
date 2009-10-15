/*
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008-2009 Appcelerator, Inc. All Rights Reserved.
 */

(function() {
	
	var ti_createProcess = Titanium.Process.createProcess;
	
	Titanium.Process.createProcess = function()
	{
		var process = ti_createProcess.apply(Titanium.Process, arguments);
		
		/**
		 * @tiapi(method=True,name=Process.Process.setOnReadLine,since=0.5)
		 * @tiarg[Function, fn] a callback that is called with every line of output received from this process
		 */
		process.setOnReadLine = function(fn)
		{
			process.buffer = '';
			process.setOnRead(function (event)
			{
				var str = event.data.toString();
				if (process.buffer.length > 0)
				{
					str = process.buffer + str;
					process.buffer = '';
				}
				
				var lines = str.split(/\r?\n/);
				var lastLine = lines[lines.length-1];
				
				if (str.indexOf(lastLine)+lastLine.length < str.length)
				{
					process.buffer = lines.pop();
				}
				
				for (var i = 0; i < lines.length; i++)
				{
					fn.apply(fn, [lines[i]]);
				}
			});
			
			process.addEventListener("exit", function (event)
			{
				if (process.buffer.length > 0)
				{
					fn(process.buffer);
					process.buffer = null;
				}
			});
		};
		
		return process;
	};
	
	/**
	 * @tiapi(method=True,name=Process.launch,since=0.2,deprecated=True)
	 * @tiapi This method is deprecated. See Process.Process.createProcess()
	 * @tiarg[String, command] The command to launch
	 * @tiarg[Array<String>, arguments] A list of arguments to the command
	 */
	Titanium.Process.launch = function(cmd, args)
	{
		Titanium.API.warn(
			"Titanium.Process.launch is deprecated, please use Titanium.Process.createProcess instead");
		
		if (!args) args = [];
		args.unshift(cmd);
		
		var process = ti_createProcess.call(Titanium.Process, args);
		var buffer = '';
		var onRead = null;
		var onExit = null;
		process.setOnRead(function(event)
		{
			if (!onRead)
			{
				buffer += event.data.toString();
			}
			else
			{
				if (buffer.length > 0)
				{
					onRead(buffer);
					buffer = '';
				}
				else
				{
					onRead(event.data.toString());
				}
			}
		});
		process.setOnExit(function(event)
		{
			if (onExit) onExit(process.getExitCode());
		});
		
		// wrap so that we can proxy the underlying Process object methods
		var processWrapper =
		{
			set onread(fn)
			{
				onRead = fn;
			},
			
			set onexit(fn)
			{
				onExit = fn;
			},
			
			terminate: function() { process.terminate(); },
			isRunning: function() { return process.isRunning(); }
		};
		
		process.launch();
		return processWrapper;
	};
})();
