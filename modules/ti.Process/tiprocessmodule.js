/*
 * Appcelerator Kroll - licensed under the Apache Public License 2
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
					buffer = lines.pop();
				}
				
				for (var i = 0; i < lines.length; i++)
				{
					fn(lines[i]);
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
	
})();