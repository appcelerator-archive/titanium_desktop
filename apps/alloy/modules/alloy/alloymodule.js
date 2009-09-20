(function(){
	Titanium.API.setLogLevel(Titanium.API.ERROR);

	var stdout = Titanium.App.stdout;
	var stdin = Titanium.App.stdin;

	stdout("~~~~~Alloy~~~~~");
	stdout("Type /<language name> to switch languages");
	stdout("Supported languages: javascript, python, ruby");
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

	var running = true;
	var activeEval = langEvals["javascript"];
	var activeMime = "text/javascript";
	var command;
	while (running)
	{
		command = stdin(">>> ");
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
		else
		{
			// evalute it as code
			var result = activeEval(activeMime, "alloy", command, this);
			if (result != undefined)
			{
				stdout(result);
			}
		}
	}

	Titanium.App.exit();
})();
