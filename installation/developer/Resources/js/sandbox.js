TiDeveloper.Sandbox = {};
TiDeveloper.Sandbox.url = "http://publisher.titaniumapp.com/api/snippet-list";
TiDeveloper.Sandbox.lastTempDir = null;
TiDeveloper.Sandbox.code = [];
TiDeveloper.Sandbox.refreshing = -1;
TiDeveloper.Sandbox.initialCodeMessage = 'Select the name of a code snippet above to see the code here.\n\nYou can also start typing code here (and optionally select a Javascript library to the left) and click \'Launch\' to run the code in a quick-launch Titanium app.';

$(document).ready(function()	
{
	$('#text_editor').markItUp(mySettings);
	
	$('#text_editor').click(function()
	{
		var selector = $('#api_selector').get(0);
		var selectedIndex = selector.selectedIndex;
		var editor = $('#text_editor');
		if (selectedIndex == 0 && editor.val()==TiDeveloper.Sandbox.initialCodeMessage)
		{
			editor.val('');
		}
	});
	
	var fetched = false;

	function refreshAPIList()
	{
		$.getJSON(TiDeveloper.Sandbox.url,function(results)
		{
			fetched = true;
			TiDeveloper.Sandbox.code = results;
			var data = [];
			data.unshift({
				title:'select code snippet...',
				code:null
			});
			for (var c=0;c<results.length;c++)
			{
				data.push({'title':results[c].title,'code':null});
			}
			$MQ('l:populate.api.selector',{rows:data});
		});
	}
	
	$('#api_refresh').click(function()
	{
		TiDeveloper.Sandbox.refreshing=$('#api_selector').get(0).selectedIndex;
		refreshAPIList();
		return false;
	});
	
	$MQL('l:menu',function(data)
	{
		if (data.payload.val == 'sandbox')
		{
			if (!fetched) refreshAPIList();
		}
	});
});


TiDeveloper.Sandbox.setAPI = function()
{
	var selector = $('#api_selector').get(0);
	var selectedIndex = selector.selectedIndex;
	if (TiDeveloper.Sandbox.refreshing!=-1 && selectedIndex == 0) 
	{
		selector.selectedIndex = TiDeveloper.Sandbox.refreshing;
		TiDeveloper.Sandbox.refreshing=-1;
		return;
	}
	if (selectedIndex > 0)
	{
		var entry = TiDeveloper.Sandbox.code[selectedIndex-1];
		var code = $.gsub(entry.code,"\\\\n","\n");
		$('#text_editor').val(code);
	}
	else
	{
		$('#text_editor').val(TiDeveloper.Sandbox.initialCodeMessage);
	}
}

$MQL('l:launch.sandbox',function(msg)
{
	var project = {};
	project.name = "sandbox";
	project.url = "http://www.titaniumapp.com";
	project.rootdir = Titanium.Filesystem.createTempDirectory().toString();
	project.dir = project.rootdir +'/'+ project.name;
	project.appid = 'com.titanium.sandbox';
	project.publisher = "Titanium";

	var jsLibs = {jquery:false,jquery_ui:false,prototype_js:false,scriptaculous:false,dojo:false,mootools:false,swf:false,yui:false};
	if ($('#jquery_js_sandbox').hasClass('selected_js'))
	{
		jsLibs.jquery = true;
	}
	if ($('#entourage_js_sandbox').hasClass('selected_js'))
	{
		jsLibs.entourage = true;
	}
	if ($('#prototype_js_sandbox').hasClass('selected_js'))
	{
		jsLibs.prototype_js = true;
	}
	if ($('#scriptaculous_js_sandbox').hasClass('selected_js'))
	{
		jsLibs.scriptaculous = true;
	}
	if ($('#dojo_js_sandbox').hasClass('selected_js'))
	{
		jsLibs.dojo = true;
	}
	if ($('#mootools_js_sandbox').hasClass('selected_js'))
	{
		jsLibs.mootools = true;
	}
	if ($('#swfobject_js_sandbox').hasClass('selected_js'))
	{
		jsLibs.swf = true;
	}
	if ($('#yahoo_js_sandbox').hasClass('selected_js'))
	{
		jsLibs.yahoo = true;
	}

	var outdir = TFS.getFile(project.rootdir,project.name);

	// remove last sandbox temp dir
	if (TiDeveloper.Sandbox.lastTempDir != null)
	{
		TiDeveloper.Sandbox.lastTempDir.deleteDirectory(true);
	}
	// record this temp dir for deletion next time
	TiDeveloper.Sandbox.lastTempDir = outdir;

	var guid = Titanium.Platform.createUUID();
	Titanium.Project.create(project.name,guid,'sandbox app',project.rootdir,project.publisher,project.url,null,jsLibs, $('#text_editor').val());
	TiDeveloper.Projects.launchProject(project,false);
	TiDeveloper.track('sandbox-launch',{jsLibs:jsLibs});
});
