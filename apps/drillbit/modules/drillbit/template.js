// ==UserScript==
// @name	Titanium App Tester
// @author	Appcelerator
// @description	Titanium Tests
// @include	app://com.titaniumapp.unittest/index.html
// @version	0.1
// ==/UserScript==

<%
	var TFS = Titanium.Filesystem;
	var TA = Titanium.App;
	
	function add_line_numbers(f)
	{
		var code = String(f);
		var lines = code.split("\n");
		var ready = false;
		var new_code = "";
		for (var linenum=0;linenum<lines.length;linenum++)
		{
			var line = lines[linenum];
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
			new_code += line+"\n";
		}
		return new_code;
	}
	
	function make_function(f,scope)
	{
		if (typeof(f)=='function')
		{
			if (typeof(scope)=='undefined')
			{
				return '(' + add_line_numbers(f) + ')();\n';
			}
			else
			{
				var expr = '(function(){var _scope = ' + scope + ';\n';
				expr+='(' + add_line_numbers(f) + ').call(_scope,_scope);\n';
				expr+='})();\n';
				return expr;
			}
		}
		return '';
	};
%>

<%= TFS.getFile(TA.appURLToPath('app://drillbit_func.js')).read() %>

TitaniumTest.NAME = "<%= entry.name %>";

try
{
	<%= make_function(entry.test.before_all, 'TitaniumTest.gscope') %>
}
catch (e)
{
	Titanium.API.error('before_all caught error:'+e+' at line: '+e.line);
}

<% for (var f in entry.test) {
	var i = excludes.indexOf(f);
	if (i == -1) { %>

		TitaniumTest.tests.push(function(){
			// <%= f %>
			var xscope = new TitaniumTest.Scope('<%= f %>');
			<%= make_function(entry.test.before,'xscope') %>;

			try {
				TitaniumTest.currentTest = '<%= f %>';
				TitaniumTest.runningTest('<%= entry.name %>', '<%= f %>');
				<%= make_function(entry.test[f], 'xscope') %>;
				<%
				i = f.indexOf('_as_async');
				if (i==-1)
				{ %>
					TitaniumTest.testPassed('<%= f %>',TitaniumTest.currentSubject.lineNumber);
				<% } %>
			}
			catch (___e)
			{
				TitaniumTest.testFailed('<%= f %>', ___e);
			}

			<%= make_function(entry.test.after, 'xscope') %>
			// --- <%= f %> ---
		});
<%	}
} %>

TitaniumTest.on_complete = function(){
	try
	{
		<%= make_function(entry.test.after_all,'TitaniumTest.gscope') %>;
	}
	catch (e)
	{
		Titanium.API.error('after_all caught error:'+e+' at line: '+e.line);
	}
	TitaniumTest.complete();
};

TitaniumTest.run_next_test();