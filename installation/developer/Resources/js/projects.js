TiDeveloper.Projects = {};

TiDeveloper.Projects.stats_url = 'http://publisher.titaniumapp.com/api/app-stats';
TiDeveloper.Projects.publish_url = 'http://publisher.titaniumapp.com/api/publish';
TiDeveloper.Projects.publish_status_url = 'http://publisher.titaniumapp.com/api/publish-status';
TiDeveloper.Projects.app_links_url = 'http://publisher.titaniumapp.com/api/app-list';
TiDeveloper.Projects.projectArray = [];
TiDeveloper.Projects.selectedProject = null;
TiDeveloper.Projects.packagingInProgress = {};
TiDeveloper.Projects.packagingError = {};

//
//  Initialization message - setup all initial states
//
$MQL('l:app.compiled',function()
{
	// load or initialize project table
	db.transaction(function(tx) 
	{   
		// see if project table exists
	   	tx.executeSql("SELECT COUNT(*) FROM Projects", [], function(result) 
	   	{
			// see if we need to run db migration
	        tx.executeSql("SELECT name,completed FROM Migrations", [], function(tx, result) 
			{
				// nope, we're good
	          	TiDeveloper.Projects.loadProjects(true); 
			},
			// create record an run it
			function(tx, error)
			{
				// run migration
				tx.executeSql('CREATE TABLE Migrations (name TEXT, completed REAL)');
				tx.executeSql('INSERT INTO Migrations values("PR3",1)');
				TiDeveloper.Projects.runGUIDMigration();
			});

	   	}, function(tx, error) 
	   	{
	       tx.executeSql("CREATE TABLE Projects (id REAL UNIQUE, guid TEXT, description TEXT, timestamp REAL, name TEXT, directory TEXT, appid TEXT, publisher TEXT, url TEXT, image TEXT)", [], function(result) 
		   { 
	          TiDeveloper.Projects.loadProjects(true); 
	       });
	   });
	});
	
});

//
// Add GUID to all projects 
//
TiDeveloper.Projects.runGUIDMigration = function()
{
	// dump all data and generate GUIDs
	db.transaction(function(tx) 
	{
        tx.executeSql("SELECT id, timestamp, name, directory, appid, publisher, url, image FROM Projects order by timestamp", [], function(tx, result) 
		{
			var a = [];
            for (var i = 0; i < result.rows.length; ++i) {
                var row = result.rows.item(i);
				var guid = Titanium.Platform.createUUID();
				var projRow = {};
				projRow['timestamp'] = row['timestamp'];
				projRow['id'] = row['id']
				projRow['guid'] = guid;
				projRow['name'] = row['name'];
				projRow['directory'] = row['directory'];
				projRow['appid'] = row['appid'];
				projRow['publisher'] = row['publisher'];
				projRow['url'] = row['url'];
				projRow['image'] = row['image'];
				projRow['description'] = row['name'] + ' is a cool new app created by ' + row['publisher'];
				a.push(projRow);
			}
			if (a.length == 0)
			{
				TiDeveloper.Projects.loadProjects(true);
			}
			// delete and re-add rows
			tx.executeSql('DROP TABLE Projects',[],function(tx,result)
			{
				// re-create table
				tx.executeSql("CREATE TABLE Projects (id REAL UNIQUE, guid TEXT, description TEXT, timestamp REAL, name TEXT, directory TEXT, appid TEXT, publisher TEXT, url TEXT, image TEXT)",[],
					function()
					{
						// re-add rows
						var rowCount =0
						for (var i=0;i<a.length;i++)
						{
							tx.executeSql('INSERT into Projects (id,guid,description,timestamp,name,directory,appid,publisher,url,image) values (?,?,?,?,?,?,?,?,?,?)',
							[a[i]['id'], a[i]['guid'],a[i]['description'],a[i]['timestamp'],a[i]['name'],a[i]['directory'],a[i]['appid'],a[i]['publisher'],a[i]['url'],a[i]['image']],
							function()
							{
								rowCount++;
								// if we're done, reload projects
								if (rowCount == a.length)
								{
									TiDeveloper.Projects.loadProjects(true);
								}
							});
						}
					});

			})

        });
	});	
}

//
// Import a project
//
TiDeveloper.Projects.importProject = function(f)
{
	var dir = f;
	var file = TFS.getFile(dir,'manifest');
	if (file.exists() == false)
	{
		alert('This directory does not contain valid Titanium project.  Please try again.');
		return;
	}
	
	// create object for DB record
	var options = {};
	options.dir = dir;
	
	// read manifest values to create new db record
	var line = file.readLine(true);
	var entry = Titanium.Project.parseEntry(line);
	for (var i=0;i<1000;i++)
	{
		if (entry == null)
		{
			line = file.readLine();
			if (!line || line == null)break;
			entry = Titanium.Project.parseEntry(line);
		}
		if (entry.key.indexOf('appname') != -1)
		{
			options.name = entry.value;
		}
		else if (entry.key.indexOf('publisher') != -1)
		{
			options.publisher = entry.value;
		}
		else if (entry.key.indexOf('url') != -1)
		{
			options.url = entry.value;
		}
		else if (entry.key.indexOf('image') != -1)
		{
			options.image = entry.value;
		}
		else if (entry.key.indexOf('appid') != -1)
		{
			options.appid = entry.value;
		}
		else if (entry.key.indexOf('guid') != -1)
		{
			options.guid = entry.value;
		}
		else if (entry.key.indexOf('desc') != -1)
		{
			options.description = entry.desc;
		}

		entry = null;
	}
	
	if (!options.description)
	{
		options.description = options.name + ' is a cool new app created by ' + options.publisher;
	}
	// if no guid, create
	if (!options.guid)
	{
		options.guid = Titanium.Platform.createUUID();
	}
	
	TiDeveloper.track('project-import',{guid:options.guid,name:options.name});

	TiDeveloper.Projects.createRecord(options,function(obj)
	{
		TiDeveloper.Projects.loadProjects();
	});
	
};
//
// Listener for Import project
//
$MQL('l:import.project',function()
{
	$MQ('l:show.filedialog',{callback:TiDeveloper.Projects.importProject});
})

//
// Update project manifest data - can be edited via developer UI
//
TiDeveloper.Projects.updateAppData = function()
{
	// write manifest
	var values = {};
	values.name = $('#project_name_value').html();
	values.publisher = $('#project_pub_value').html();
	values.dir = $('#project_dir_value').html();
	values.image = $('#project_pub_image_value').html();
	values.url = $('#project_pub_url_value').html();
	values.description = $('#project_desc_value').html();

	var id = $('#project_id_value').get(0).value;

	// update database
    db.transaction(function (tx) 
    {
        tx.executeSql("UPDATE Projects set name = ?, description = ?, directory = ?, publisher = ?, url = ?, image = ? WHERE id = ?", 
		[values.name,values.description,values.dir,values.publisher,values.url,values.image, id]);
    });
	
	// update our array cache
	var project = TiDeveloper.Projects.findProjectById(id);
	project.name = values.name;
	project.dir = values.dir;
	project.publisher = values.publisher;
	project.url = values.url;
	project.image = values.image;
	project.description = values.description;
	
};

$MQL('l:proj_data',function(msg)
{
	if (msg.payload.val == 'stats')
	{
		TiDeveloper.Projects.refreshStats(TiDeveloper.Projects.selectedProject.guid);
	}
	
})

//
// Refresh project packages 
//
TiDeveloper.Projects.refreshPackages = function(guid)
{
	var array = [];
	var date = null;
	var pageUrl = null;
	
	//
	// GENRIC FUNCTION FOR POPULATING ARRAY
	//
	function setRows(rows,pubdate)
	{
		// reset array
		array = [];

		// cycle through downloads
       	for (var i = 0; i < rows.length; ++i) 
	   	{
			// is db record or JSON
            var row = (rows.item)?rows.item(i):rows[i];

			date = (pubdate)?pubdate:row['date']

			var url = row['url'];
			var label = row['label'];
			var platform = row['platform'];
			var version = row['version'];
			if (!pageUrl)
			{
				pageUrl = row['page_url']
			}
			array.push({'url':url,'label':label,'platform':platform});
		}
	}

	//
	// GET DATA FROM DB
	//
    db.transaction(function (tx) 
    {
        tx.executeSql("SELECT url, page_url,label,platform, version, date from ProjectPackages WHERE guid = ?",[guid],
 			function(tx,result)
			{
				if (result.rows.length != 0)
				{
					setRows(result.rows)
				}
			},
			function(error)
			{
				// create table
				tx.executeSql('CREATE TABLE ProjectPackages (guid TEXT, label TEXT, url TEXT, platform TEXT, version TEXT, date TEXT,page_url TEXT)');
			}
		);
    });

	//
	// CHECK FOR DATA IN THE CLOUD
	//
	var url = TiDeveloper.make_url(TiDeveloper.Projects.app_links_url,{
		'guid':guid
	});
	$.getJSON(url,function(r)
	{
		if (r.releases)
		{
			TiDeveloper.Projects.insertPackagingRows(guid,r.pubdate,r.releases,r.app_page);
			date = TiDeveloper.Projects.formatPackagingDate(r.pubdate);
			pageUrl = r.app_page;
			setRows(r.releases,date);
			
			// send message and set UI state
			$MQ('l:package_links',{'date':date, 'rows':array});
			$('#all_download_link').attr('href',pageUrl);
			$('#all_download_link').html(pageUrl);						
			$('#packaging_none').css('display','none');
			$('#packaging_listing').css('display','block');
			$('#packaging_in_progress').css('display','none');
			$('#packaging_error').css('display','none');		

		}
		else
		{
			$('#packaging_none').css('display','block');
			$('#packaging_error').css('display','none');		
			$('#packaging_listing').css('display','none');
			$('#packaging_in_progress').css('display','none');
			
		}
	});
};

//
// Refresh project download stats
//
TiDeveloper.Projects.refreshStats = function(guid)
{
	var statsArray = [];
	var statsLastUpdate = null;
	var statsAvailable = false;
	var topValue = 0;
    db.transaction(function (tx) 
    {
    	tx.executeSql("SELECT platform, count, date from ProjectDownloads WHERE guid = ?",[guid],
 			function(tx,result)
			{
				var date = null;
				// cycle through downloads
	           	for (var i = 0; i < result.rows.length; ++i) 
			   	{
	                var row = result.rows.item(i);
					var platform = row['platform'];
					var count = row['count'];
					if (count >= topValue)topValue = count;
					
				 	statsLastUpdate= row['date'];
					statsArray.push({'name':platform,'value':count,});
				}
				if (result.rows.length > 0) statsAvailable = true;
			},
			function(error)
			{
				// create table
				tx.executeSql('CREATE TABLE ProjectDownloads (guid TEXT, platform TEXT, count TEXT, date TEXT)');
			}
		);
    });

	// load stats
	$('#download_stats_none').css('display','none');
	$('#download_stats').css('display','none');
	$('#download_stats_loading').css('display','block');
	
	var url = TiDeveloper.make_url(TiDeveloper.Projects.stats_url,{
		'guid':guid
	});
	
	$.ajax({
		type:'GET',
		dataType:'json',
		url:url,
		
		// update data
		success: function(data)
		{
			var aguid = guid;
			if (data.length > 0)
			{

			    db.transaction(function (tx) 
			    {
			    	tx.executeSql("DELETE from ProjectDownloads WHERE guid = ?",[aguid], function(tx, result)
					{
						var date = new Date().toLocaleString();
						var pageUrl = data.app_page
						var a = [];
						var totalCount = 0
						for (var i=0; i< data.length;i++)
						{
							tx.executeSql("INSERT into ProjectDownloads (guid, platform, count,date,page_url) values (?,?,?,?,?)",[aguid,data[i]['os'],data[i]['count'], date,pageUrl]);
							var platform = data[i]['os'];
							var count = data[i]['count'];
							if (count >= topValue)topValue = count;
							totalCount += count;
							a.push({name:platform,value:count,guid:aguid});
						}
						if (totalCount == 0)
						{
							$('#download_stats_none').css('display','block')
							$('#download_stats').css('display','none');
							$('#download_stats_loading').css('display','none');
						}
						else
						{
							$('#download_stats_none').css('display','none')
							$('#download_stats').css('display','block');
							$('#download_stats_loading').css('display','none');
							
							topValue = parseInt(topValue*1.2);
							var mid = parseInt(topValue/2);
							var imageUrl = "http://chart.apis.google.com/chart?chxt=x,y&chxl=0:|Windows|Linux|OSX|1:|0|"+mid+"|"+topValue+"&cht=bvs&chd=t:"+data[0]['count']+","+data[1]['count']+","+data[2]['count']+"&chds=0,"+topValue+"&chco=76A4FB&chls=2.0&chs=300x200&chbh=r,0.3&chf=bg,s,333333"
							var img = document.createElement('img')
							img.src = imageUrl;
							$('#google_chart').html(img);
							
							//$MQ('l:package_download_stats',{date:date, rows:a})
	
						}
					});
				});
			}
			else
			{
				$('#download_stats_none').css('display','block')
				$('#download_stats').css('display','none');
				$('#download_stats_loading').css('display','none');
				
			}
		},
		error: function()
		{
			// if we have them, send message
			if (statsAvailable==true)
			{
				$('#download_stats_none').css('display','none');
				$('#download_stats').css('display','block');
				$('#download_stats_loading').css('display','none');

				topValue = parseInt(topValue*1.2);
				var mid = parseInt(topValue/2);
				var imageUrl = "http://chart.apis.google.com/chart?chxt=x,y&chxl=0:|Windows|Linux|OSX|1:|0|"+mid+"|"+topValue+"&chd=t:"+statsArray[0]['count']+","+statsArray[1]['count']+","+statsArray[2]['count']+"&chds=0,"+topValue+"&chco=76A4FB&chls=2.0&chs=300x200&chbh=r,0.3&chf=bg,s,333333"
				var img = document.createElement('img')
				img.src = imageUrl;
				$('#google_chart').html(img);

//				$MQ('l:package_download_stats',{date:statsLastUpdate, rows:statsArray})
			}
			else
			{
				//TODO:remove
				setTimeout(function()
				{
					$('#download_stats_none').css('display','block');
					$('#download_stats').css('display','none');
					$('#download_stats_loading').css('display','none');
					
				},1000);
			}
		}
	});
	
};

//
// Refresh Download stats
//
$MQL('l:refresh_downlaod_stats',function(msg)
{
	TiDeveloper.Projects.refreshStats(TiDeveloper.Projects.selectedProject.guid)
})

//
// Set row selection listener for project list
//
$MQL('l:row.selected',function(msg)
{
	var msgObj = {};
	var project = TiDeveloper.Projects.findProjectById(msg.payload.project_id);
	TiDeveloper.Projects.selectedProject = project;	
	msgObj.date = project.date;
	msgObj.name = project.name;
	msgObj.location = TiDeveloper.Projects.formatDirectory(project.dir);
	msgObj.fullLocation = project.dir;
	msgObj.pub = project.publisher;
	msgObj.url = project.url;
	msgObj.image = project.image;
	msgObj.description = project.description;
	$MQ('l:project.detail.data',msgObj);
	
	// 
	// UPDATE STATS and DOWNLOADS TABS
	//
	if (TiDeveloper.Projects.packagingInProgress[project.guid] == true)
	{
		$('#packaging_none').css('display','none');
		$('#packaging_error').css('display','none');		
		$('#packaging_listing').css('display','none');
		$('#packaging_in_progress').css('display','block');
		
	}
	else if (TiDeveloper.Projects.packagingError[project.guid] == true)
	{
		$('#packaging_none').css('display','none');
		$('#packaging_error').css('display','block');		
		$('#packaging_listing').css('display','none');
		$('#packaging_in_progress').css('display','none');
		
	}
	else
	{
		TiDeveloper.Projects.refreshPackages(project.guid)
	}
	
	TiDeveloper.Projects.refreshStats(project.guid);
	
	// setup editable fields for INFO tab
	$('.edit').click(function()
	{
		if ($(this).attr('edit_mode') != 'true')
		{
			// only one active edit field at a time
			var activeFiles = $('div[edit_mode=true]');
			if (activeFiles)
			{
				for (var i=0;i<activeFiles.length;i++)
				{
					var id = $(activeFiles[i]).attr('id');
					$(activeFiles[i]).html($('#'+id+'_input').val());
					TiDeveloper.Projects.updateAppData();
					$(activeFiles[i]).get(0).removeAttribute('edit_mode');
				}
			}
			
			// process click
			var el = $(this).get(0);

			// if field requires file dialog - show
			if (el.id == 'project_pub_image_value')
			{
				// show dialog
				$MQ('l:show.filedialog',{'for':'project_image','target':'project_pub_image_value'});
				
				// listen for value selection
				$MQL('l:file.selected',function(msg)
				{
					var target = msg.payload.target;
					if (target=='project_pub_image_value')
					{
						el.removeAttribute('edit_mode');
						TiDeveloper.Projects.updateAppData();
					}
				});
			}
			var value = el.innerHTML;
			el.setAttribute('edit_mode','true');
			
			// create input and focus
			$(this).html('<input id="'+el.id+'_input" value="'+value+'" type="text" style="width:350px" maxlength="150"/>');
			$('#'+el.id+'_input').focus();
			
			// listen for enter
			$('#'+el.id+'_input').keyup(function(e)
			{
				if (e.keyCode==13)
				{
					el.innerHTML = $('#'+el.id+'_input').val();
					el.removeAttribute('edit_mode');
					TiDeveloper.Projects.updateAppData();
				}
				else if (e.keyCode==27)
				{
					el.innerHTML = value; 
					el.removeAttribute('edit_mode');
				}
			});

		}
	});
	
});

//
// Create a project record in the DB and update array cache
//
TiDeveloper.Projects.createRecord = function(options,callback)
{
	var date = new Date();
	var dateStr = (date.getMonth()+1)+"/"+date.getDate()+"/"+date.getFullYear();
	var record = {
		name: options.name,
		dir: String(options.dir),
		id: ++TiDeveloper.highestId,
		appid: options.appid,
		date: dateStr,
		publisher:options.publisher,
		url:options.url,
		image:options.image,
		guid:options.guid,
		description:options.description
	};
    db.transaction(function (tx) 
    {
        tx.executeSql("INSERT INTO Projects (id, guid, description,timestamp, name, directory, appid, publisher, url, image) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", [record.id,record.guid,record.description,date.getTime(),record.name,record.dir,record.appid,record.publisher,record.url,record.image]);
    },
	function(error)
	{
		alert('error insert ' + error);
		callback({code:1,id:error.id,msg:error.message});
	},
	function()
	{
		TiDeveloper.Projects.projectArray.push(record);
		callback({code:0});
	});
}

//
// load projects from db and populate array cache
//
TiDeveloper.Projects.loadProjects = function(init)
{
	db.transaction(function(tx) 
	{
        tx.executeSql("SELECT id, guid, description, timestamp, name, directory, appid, publisher, url, image FROM Projects order by timestamp", [], function(tx, result) 
		{
			TiDeveloper.Projects.projectArray = [];
            for (var i = 0; i < result.rows.length; ++i) {
                var row = result.rows.item(i);
				// check to see if the user has deleted it and if
				// so remove it
				var cd = TFS.getFile(row['directory']);
				if (!cd.exists())
				{
					tx.executeSql('DELETE FROM Projects where id = ?',[row['id']]);
				}
				else
				{
					var date = new Date();
					date.setTime(row['timestamp']);
					
					TiDeveloper.Projects.projectArray.push({
						id: row['id'],
						date: (date.getMonth()+1)+"/"+date.getDate()+"/"+date.getFullYear(),
						name: row['name'],
						dir: row['directory'],
						appid: row['appid'],
						publisher: row['publisher'],
						url: row['url'],
						image: row['image'],
						guid: row['guid'],
						description:row['description']
					});
					if (TiDeveloper.highestId < row['id'])
					{
						TiDeveloper.highestId = row['id'];
					}
				}
            }
			TiDeveloper.currentPage = 1;
			var data = TiDeveloper.Projects.getProjectPage(10,TiDeveloper.currentPage);
			var count = TiDeveloper.formatCountMessage(TiDeveloper.Projects.projectArray.length,'project');
			
			// show create project if none
			if (TiDeveloper.Projects.projectArray.length == 0)
			{
				$MQ('l:menu',{val:'manage'});
			}
			
			$('#project_count_hidden').val(TiDeveloper.Projects.projectArray.length);
			var firstCall = (init)?1:0;
			$MQ('l:project.list.response',{firstCall:firstCall,count:count,page:TiDeveloper.currentPage,totalRecords:TiDeveloper.Projects.projectArray.length,'rows':data});
        });
	});	
}

//
//  create.project service
//
$MQL('l:create.project.request',function(msg)
{
	try
	{
		var jsLibs = {jquery:false,jquery_ui:false,prototype_js:false,scriptaculous:false,dojo:false,mootools:false,swf:false,yui:false};
		if ($('#jquery_js').hasClass('selected_js'))
		{
			jsLibs.jquery = true;
		}
		if ($('#entourage_js').hasClass('selected_js'))
		{
			jsLibs.entourage = true;
		}
		if ($('#prototype_js').hasClass('selected_js'))
		{
			jsLibs.prototype_js = true;
		}
		if ($('#scriptaculous_js').hasClass('selected_js'))
		{
			jsLibs.scriptaculous = true;
		}
		if ($('#dojo_js').hasClass('selected_js'))
		{
			jsLibs.dojo = true;
		}
		if ($('#mootools_js').hasClass('selected_js'))
		{
			jsLibs.mootools = true;
		}
		if ($('#swfobject_js').hasClass('selected_js'))
		{
			jsLibs.swf = true;
		}
		if ($('#yahoo_js').hasClass('selected_js'))
		{
			jsLibs.yahoo = true;
		}

		var guid = Titanium.Platform.createUUID();
		
		// set default image if not specified
		if (msg.payload.image=="")
		{
			msg.payload.image = 'default_app_logo.png';
		}
		
		var result = Titanium.Project.create(msg.payload.project_name,guid,msg.payload.description,msg.payload.project_location,msg.payload.publisher,msg.payload.url,msg.payload.image,jsLibs);
		if (result.success)
		{
			var options = {name:result.name, guid:guid,description:msg.payload.description,dir:result.basedir,appid:result.id,publisher:msg.payload.publisher,url:msg.payload.url,image:msg.payload.image}
			TiDeveloper.track('project-create',{name:msg.payload.project_name,description:msg.payload.description,publisher:msg.payload.publisher,guid:guid,url:msg.payload.url,jsLibs:jsLibs});
			var r = TiDeveloper.Projects.createRecord(options,function(obj)
			{
				if (obj.code == 0)
				{
					$MQ('l:create.project.response',{result:'success'});
					var count = TiDeveloper.formatCountMessage(TiDeveloper.Projects.projectArray.length,'project');
					$MQ('l:project.list.response',{count:count,page:1,totalRecords:TiDeveloper.Projects.projectArray.length,'rows':TiDeveloper.Projects.projectArray})
				}
				else
				{
					$MQ('l:create.project.response',{result:'error',msg:obj.msg});
				}
			});		
		}
		else
		{
			$MQ('l:create.project.response',{result:'error',msg:result.message});
		}
	}
	catch(E)
	{
		$MQ('l:create.project.response',{result:'error',msg:E});
	}
});

//
// Handling paging requests
//
$MQL('l:page.data.request',function(msg)
{
	// paging gets called in both search and list
	// cases - if search yields 0 results, do nothing
	if (TiDeveloper.Projects.projectArray.length == 0)return;
	
	var state =msg.payload;
	var rowsPerPage = state.rowsPerPage;
	var page = state.page;
	TiDeveloper.currentPage = page;
	var data = TiDeveloper.Projects.getProjectPage(rowsPerPage,page);
	var count = TiDeveloper.formatCountMessage(TiDeveloper.Projects.projectArray.length,'project');
	
	$MQ('l:project.list.response',{count:count,page:page,totalRecords:TiDeveloper.Projects.projectArray.length,'rows':data})
});


//
// Format directory string for display purposes
//
TiDeveloper.Projects.formatDirectory =function(dir)
{
	
	if (dir != null)
	{
		var dirStr = dir;
		if (dir.length > 70)
		{
			dirStr = dir.substring(0,70) + '...';
			$('#project_detail_dir_a').css('display','block');
			$('#project_detail_dir_span').css('display','none');
		}
		else
		{
			$('#project_detail_dir_span').css('display','block');
			$('#project_detail_dir_a').css('display','none');
		}
	}
	return dirStr;
}
//
// Get a page of data
//
TiDeveloper.Projects.getProjectPage = function(pageSize,page)
{
	var pageData = [];
	var start = (page==0)?0:(pageSize * page) - pageSize;
	var end = ((pageSize * page) > TiDeveloper.Projects.projectArray.length)?TiDeveloper.Projects.projectArray.length:(pageSize * page);
	for (var i=start;i<end;i++)
	{
		pageData.push(TiDeveloper.Projects.projectArray[i]);
	}
	return pageData;
};


//
//  Handle Package Project Button - enable/disable
//
$MQL('l:os_platform_click',function()
{
	if ($('.selected_os').length > 0)
	{
		$('#package_project_button').attr('disabled','false');
	}
	else
	{
		$('#package_project_button').attr('disabled','true');
	}
})

//
//  Project Package Request - get details about modules, etc
//
$MQL('l:package.project.request',function(msg)
{
	var project = TiDeveloper.Projects.findProjectById(msg.payload.id);
	Titanium.Project.setModules(project.dir);
	$MQ('l:package.project.data',{rows:Titanium.Project.optionalModules});
 	$MQ('l:package.all',{val:'network'});

});

//
// Find project by name
//
TiDeveloper.Projects.findProject = function(name)
{
	for (var i=0;i<TiDeveloper.Projects.projectArray.length;i++)
	{
		if (TiDeveloper.Projects.projectArray[i].name == name)
		{
			return TiDeveloper.Projects.projectArray[i];
		}
	}
	return null;
}

//
// Find project by ID
//
TiDeveloper.Projects.findProjectById = function(id)
{
	for (var i=0;i<TiDeveloper.Projects.projectArray.length;i++)
	{
		if (TiDeveloper.Projects.projectArray[i].id == id)
		{
			return TiDeveloper.Projects.projectArray[i];
		}
	}
	return null;
}

//
// Get a project name by id
//
TiDeveloper.Projects.getProjectName = function(id)
{
	var p =  TiDeveloper.Projects.findProjectById(id);
	return (p)?p.name:'Project Not Found';
}


//
// Launch or launch and install project locally
//
TiDeveloper.Projects.launchProject = function(project, install)
{
	Titanium.Project.launch(project,install);
}

$MQL('l:launch.project.request',function(msg)
{
	var project_name = $('#package_project_name').html();
	var project = TiDeveloper.Projects.findProject(project_name);
	TiDeveloper.Projects.launchProject(project,false);
});

$MQL('l:launch.project.installer.request',function(msg)
{
	var project_name = $('#package_project_name').html();
	var project = TiDeveloper.Projects.findProject(project_name);
	TiDeveloper.Projects.launchProject(project,true);
});

$MQL('l:show.package.project',function()
{
	$('#package_public').get(0).checked = false;
});
$MQL('l:package.project.request',function()
{
	$('#package_public').get(0).checked = false;
});

//
// Create Package Request
//
$MQL('l:create.package.request',function(msg)
{
	try
	{
		// project name and project
		var project_name = $('#package_project_name').html();
		var project = TiDeveloper.Projects.findProject(project_name);

		// make sure required files/dirs are present
		var resources = TFS.getFile(project.dir,'Resources');
		if (!resources.exists())
		{
			alert('Your project is missing the Resources directory.  This directory is required for packaging.');
			return;
		}
		var tiapp = TFS.getFile(project.dir,'tiapp.xml');
		if (!tiapp.exists())
		{
			alert('Your tiapp.xml file is missing.  This file is required for packaging.');
			return;
		}

		// base runtime option
		var networkRuntime = ($('#required_modules_network').attr('checked') ==true)?'network':'include';

		// elements that are included for network bundle
		var networkEl = $("div[state='network']");

		// elements that are included (bundled)
		var bundledEl = $("div[state='bundled']");

		// elements that are excluded
		var excludedEl = $("div[state='exclude']");
		
		// capture excluded modules
		var excluded = {};
		$.each(excludedEl,function()
		{
			var key = $.trim($(this).html());
			excluded[key]=true;
		});

		// write out manifest
		var manifest = Titanium.Project.writeManifest(project,excluded);

		var timanifest = {};

		//
		// Write out TIMANIFEST
		//
		timanifest.appname = project_name;
		timanifest.appid = project.appid;
		timanifest.appversion = "1.0";
		timanifest.mid = Titanium.Platform.id;
		timanifest.publisher = project.publisher;
		timanifest.url = project.url;
		timanifest.desc = project.description;
		timanifest.image = imageName;

		// OS options
		timanifest.platforms = [];
		if ($('#platform_mac').hasClass('selected_os'))
		{
			timanifest.platforms.push('osx');
		}
		if ($('#platform_windows').hasClass('selected_os'))
		{
			timanifest.platforms.push('win32');
		}
		if ($('#platform_linux').hasClass('selected_os'))
		{
			timanifest.platforms.push('linux');
		}
		var visibility = ($('#package_public').attr('checked')==true)?'public':'private';

		timanifest.visibility = visibility;

		timanifest.runtime = {};
		timanifest.runtime.version = "" + Titanium.Project.runtimeVersion;
		timanifest.runtime.package = networkRuntime;

		timanifest.guid = project.guid;
		timanifest.modules = [];
		
		// required modules
		var requiredModulesPackaging = networkRuntime; // 'network' or 'include'
		for (var i=0;i<Titanium.Project.requiredModules.length;i++)
		{
			var m = {};
			m.name = Titanium.Project.requiredModules[i].name;			
			m.version = "" + Titanium.Project.requiredModules[i].version;
			m.package = requiredModulesPackaging;
			timanifest.modules.push(m);
		}

		// write out optional modules
		for (var c=0;c<Titanium.Project.optionalModules.length;c++)
		{
			var module = Titanium.Project.optionalModules[c].name;
			var version = "" + Titanium.Project.optionalModules[c].version;
			
			$.each(excludedEl,function()
			{
				var key = $.trim($(this).html());
				if (key == module)
				{
					var m = {};
					m.name = module;
					m.version = "" + version;
					m.package = 'exclude';
					timanifest.modules.push(m);
				}
			});
			$.each(bundledEl,function()
			{
				var key = $.trim($(this).html());
				if (key == module)
				{
					var m = {};
					m.name = module;
					m.version = "" + version;
					m.package = 'include';
					timanifest.modules.push(m);
				}
			});
			$.each(networkEl,function()
			{
				var key = $.trim($(this).html());
				if (key == module)
				{
					var m = {};
					m.name = module;
					m.version = "" + version;
					m.package = 'network';
					timanifest.modules.push(m);
				}
			});
			
		}
		var timanifestFile = TFS.getFile(project.dir,'timanifest');
		timanifestFile.write(swiss.toJSON(timanifest));
				
		//
		// NOW CREATE TEMP DIR AND MOVE CONTENTS FOR PACKAGING
		//
		
		var destDir = Titanium.Filesystem.createTempDirectory();
		var modules = TFS.getFile(project.dir,'modules');
		var timanifest = TFS.getFile(project.dir,'timanifest');
		var manifest = TFS.getFile(project.dir,'manifest');
		
		// copy files to temp dir
		var resDir = TFS.getFile(destDir,'Resources');
		resDir.createDirectory();
		
		//FIXME: we can't do this ... async may not finish before you
		//go to package
		
		TFS.asyncCopy(resources, resDir,function(path,currentIndex,total)
		{
			if (currentIndex==total)
			{
				TFS.asyncCopy([tiapp,timanifest,manifest], destDir,function(path,currentIndex,total)
				{
					if (currentIndex==total)
					{
						// if project has modules, copy
						if (modules.exists())
						{
							// create resources dir
							var resDir = TFS.getFile(destDir,'modules');
							resDir.createDirectory();
							TFS.asyncCopy(modules, resDir,function(path,currentIndex,total)
							{
								if (currentIndex==total)
								{
									publish();
								}
							});
						}
						else
						{
							publish();
						}
						
						function publish()
						{
							// packaging request
							var xhr = Titanium.Network.createHTTPClient();
							var ticket = null;
							xhr.onreadystatechange = function()
							{
								// 4 means that the POST has completed
								if (this.readyState == 4)
								{
									if (this.status == 200)
									{
										var json = null;
										try
										{
										    json = swiss.evalJSON(this.responseText);
										}
										catch (e)
										{
											$('#packaging_error_msg').html('Invalid JSON response from service');
											$('#packaging_none').css('display','none');
											$('#packaging_listing').css('display','none');
											$('#packaging_error').css('display','block');		
											$('#packaging_in_progress').css('display','none');
											
										}
										if (json.success == false)
										{
											$('#packaging_error_msg').html(json.message);
											$('#packaging_none').css('display','none');
											$('#packaging_listing').css('display','none');
											$('#packaging_error').css('display','block');		
											$('#packaging_in_progress').css('display','none');
										}
										else
										{
											TiDeveloper.Projects.pollPackagingRequest(json.ticket,project.guid);
										}
										destDir.deleteDirectory(true);
									}
									else
									{
										$('#packaging_error_msg').html('Unexpected error. Please try again later.');
										$('#packaging_none').css('display','none');
										$('#packaging_listing').css('display','none');
										$('#packaging_error').css('display','block');		
										$('#packaging_in_progress').css('display','none');

										TiDeveloper.Projects.packagingInProgress[project.guid] = false;
										TiDeveloper.Projects.packagingError[project.guid] = true;
										destDir.deleteDirectory(true);
									}
								}
							};

							xhr.open("POST",TiDeveloper.Projects.publish_url);
							xhr.sendDir(destDir);    

							TiDeveloper.Projects.packagingInProgress[project.guid] = true;
							TiDeveloper.Projects.packagingError[project.guid] = false;

							$('#packaging_none').css('display','none');
							$('#packaging_listing').css('display','none');
							$('#packaging_error').css('display','none');		
							$('#packaging_in_progress').css('display','block');
							$MQ('l:create.package.response',{result:0});
							
						};
					}
				});		
			}
			
		});		
	}
	catch(E)
	{
		alert("Exception = "+E);
	}
});
TiDeveloper.Projects.formatPackagingDate = function(str)
{
	var parts = str.split(' ');
	var time = TiDeveloper.Feeds.convertDate(parts[1]);
	var date = parts[0].split('-');
	return date[1] + '/' + date[2] + '/' + date[0] + ' ' + time;
}
TiDeveloper.Projects.insertPackagingRows = function(guid,date,rows,pageUrl)
{
	var date = TiDeveloper.Projects.formatPackagingDate(date);
    db.transaction(function (tx) 
    {
        tx.executeSql("DELETE from ProjectPackages WHERE guid = ?",[guid], function(tx,result)
		{
			for (var i=0;i<rows.length;i++)
			{
		        tx.executeSql("INSERT INTO ProjectPackages (guid,url, label,platform, version, date,page_url) values (?,?,?,?,?,?,?) ",[guid,rows[i]['url'],rows[i]['label'],rows[i]['platform'],rows[i]['version'],date,pageUrl]);
			}
		});

	});
	
};

TiDeveloper.Projects.pollPackagingRequest = function(ticket,guid)
{          
	var url = TiDeveloper.make_url(TiDeveloper.Projects.publish_status_url,{
		'ticket':ticket
	});
	$.getJSON(url,function(r)
	{
	   	if (r.status == 'complete')
	   	{
  			TiDeveloper.Projects.packagingInProgress[guid] = false;
			TiDeveloper.Projects.insertPackagingRows(guid,r.pubdate,r.releases,r.app_page);
			
			$('#all_download_link').attr('href',r.app_page);
			$('#all_download_link').html(r.app_page);						
			
			$MQ('l:package_links',{date:r.pubdate,rows:r.releases})
			$('#packaging_none').css('display','none');
			$('#packaging_error').css('display','none');
			$('#packaging_listing').css('display','block');
			$('#packaging_in_progress').css('display','none');
		}
		else if (r.success == false)
		{
			$('#packaging_error_msg').html(r.message);
			$('#packaging_none').css('display','none');
			$('#packaging_listing').css('display','none');
			$('#packaging_error').css('display','block');		
			$('#packaging_in_progress').css('display','none');
			return;
			
		}
		else
		{
			// poll every 10 seconds
			setTimeout(function()
			{
				TiDeveloper.Projects.pollPackagingRequest(ticket,guid);
			},10000);
		}
	});
};
//
//  Delete a project
//	
$MQL('l:delete.project.request',function(msg)
{
	var name = msg.payload.name;
	var id = msg.payload.project_id;
	var project = TiDeveloper.Projects.findProjectById(id);
	var file = Titanium.Filesystem.getFile(project.dir);

	file.deleteDirectory(true);
	
	db.transaction(function (tx) 
    {
        tx.executeSql("DELETE FROM Projects where id = ?", [id]);
		TiDeveloper.Projects.loadProjects();
    });

	TiDeveloper.track('project-delete',{name:name,guid:project.guid});
});

//
// project search request
//
$MQL('l:project.search.request',function(msg)
{
	var q = msg.payload.search_value;
	db.transaction(function(tx) 
	{
		try
		{
	        tx.executeSql("SELECT id, guid, description, timestamp, appid, publisher, url, image, name, directory FROM Projects where name LIKE '%' || ? || '%'", [q], function(tx, result) 
			{
				try
				{
					TiDeveloper.Projects.projectArray = [];
		            for (var i = 0; i < result.rows.length; ++i) 
					{
		                var row = result.rows.item(i);
						var date = new Date();
						date.setTime(row['timestamp']);
						TiDeveloper.Projects.projectArray.push({
							id: row['id'],
							date: (date.getMonth()+1)+"/"+date.getDate()+"/"+date.getFullYear(),
							name: row['name'],
							dir: row['directory'],
							appid: row['appid'],
							publisher: row['publisher'],
							url: row['url'],
							image: row['image'],
							guid:row['guid'],
							description:row['description']
						});
					}
					$MQ('l:project.search.response',{count:TiDeveloper.Projects.projectArray.length,page:1,totalRecords:TiDeveloper.Projects.projectArray.length,'rows':TiDeveloper.Projects.projectArray});
				}
				catch (EX)
				{
					alert("EXCEPTION = "+EX);
				}
			});
		}
		catch (E)
		{
			alert("E="+e);
		}
	});
	TiDeveloper.track('project-search',{'q':q});
});


//
// Show file dialog and send value
//
$MQL('l:show.filedialog',function(msg)
{
	var el = msg.payload['for'];
	var target = msg.payload.target;
	var props = {multiple:false};
	if (el == 'project_image')
	{
		props.directories = false;
		props.files = true;
		props.types = ['gif','png','jpg'];
	}
	else
	{
		props.directories = true;
		props.files = false;
	}
	
	Titanium.UI.openFiles(function(f)
	{
		if (f.length)
		{
			$MQ('l:file.selected',{'target':target,'for':el,'value':f[0]});
			if (msg.payload.callback)
			{
				msg.payload.callback(f[0]);
			}
		}
	},
	props);
});

