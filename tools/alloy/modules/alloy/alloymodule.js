(function(){
	Titanium.API.setLogLevel(Titanium.API.ERROR);

	var stdout = Titanium.App.stdout;
	var stdin = Titanium.App.stdin;

	String.prototype.rtrim=function(){return this.replace(/\s+$/,'');}
	String.prototype.endsWith = function endsWith(pattern)
	{
    	var d = this.length - pattern.length;
    	return d >= 0 && this.lastIndexOf(pattern) === d;
  	}

	stdout("~~~~~Alloy~~~~~");
	stdout("Type /<language name> to switch languages");
	stdout("Supported languages: javascript, python, ruby");
	stdout("For multi-line blocks of code, use a \\ to continue on a newline");
	stdout("Type /exit to quit");
	stdout("");

	var langEvals = {
		javascript: function(mimeType, scriptName, code, globals)
		{
			try
			{
				eval(code);
			}
			catch(e)
			{
				stdout("Javascript error: " + e);
			}
		},
		python: Titanium.Python.evaluate,
		ruby: Titanium.Ruby.evaluate,
	};

	var activeEval = langEvals["javascript"];
	var activeMime = "text/javascript";
	var command = "";
	while (true)
	{
		command += stdin(">>> ").rtrim();
		if (command == "/exit")
		{
			break;
		}
		else if (command.charAt(0) == '/')
		{
			// Switch languages
			var lang = command.substr(1).toLowerCase();
			var langEval = langEvals[lang];
			if (langEval != undefined)
			{
				activeMime = "text/" + lang;
				activeEval = langEvals[lang];
			}
			else
			{
				stdout("Invalid language: " + lang);
			}
		}
		else if (command.endsWith("\\"))
		{
			// multi-line code
			command = command.substring(0, command.length - 1) + "\n";
			continue;
		}
		else
		{
			// evalute it as code
			var result = activeEval(activeMime, "alloy", command, this);
			if (result != undefined)
			{
				stdout(result);
			}
		}
		command = "";
	}

	Titanium.App.exit();
})();
