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
		process.setOnReadLine = function(fn)
		{
			var buffer = '';
			process.setOnRead(function (event)
			{
				var str = event.data.toString();
				if (buffer.length > 0)
				{
					str = buffer + str;
				}
				
				var newline = str.lastIndexOf("\n");
				if (newline >= 0 && newline < str.length - 1)
				{
					buffer += str.substring(newline+1);
					str = str.substring(0, newline);
				}
				else if (newline < 0)
				{
					buffer += str;
					return;
				}
				
				if (str.length > 0)
				{
					var lines = str.split(/\r?\n/);
					for (var i = 0; i < lines.length; i++)
					{
						fn(line);
					}
				}
			});
			
			process.addEventListener("exit", function (event)
			{
				if (buffer.length > 0)
				{
					fn(buffer);
					buffer = null;
				}
			});
		};
		
		return process;
	};
	
})();