TiDeveloper.Feeds = {};
TiDeveloper.Feeds.twitterUsername = null;
TiDeveloper.Feeds.twitterPassword = null;
TiDeveloper.Feeds.twitterFollowing = false

//
//  Initialization message - setup all initial states
//
$MQL('l:app.compiled',function()
{
	// load or initialize twitter credentials
	db.transaction(function(tx) 
	{
	   tx.executeSql("SELECT username,password,following FROM Twitter", [], function(tx,result) 
	   {
		   for (var i = 0; i < result.rows.length; ++i) 
		   {
                var row = result.rows.item(i);
				TiDeveloper.Feeds.twitterUsername = row['username'];
				TiDeveloper.Feeds.twitterPassword = row['password'];
				TiDeveloper.Feeds.twitterFollowing = (row['following']==0)?false:true;
				$('#twitter_username').val(TiDeveloper.Feeds.twitterUsername);
				$('#twitter_password').val(TiDeveloper.Feeds.twitterPassword);
				swiss('#twitter_username').fire('revalidate')
				swiss('#twitter_password').fire('revalidate')
				break;
			}
	   }, function(tx, error) 
	   {
	       tx.executeSql("CREATE TABLE Twitter (id REAL UNIQUE, following INT, username TEXT, password TEXT)");
	   })
	});
	
	// load twitter feed
	TiDeveloper.Feeds.loadTwitter();
	
});


//
// Convert feed date strings
//
TiDeveloper.Feeds.convertDate = function(str)
{
	var parts = str.split(':');
	var hour = parseInt(parts[0]);
	var minutes = parts[1];
	var ampm = 'am';
	if (hour > 12)
	{
		hour = hour - 12
		ampm = 'pm';
	}
	else if (hour == 0)
	{
		hour = 12;
	}
	return hour + ":" + minutes + ampm;
}

//
// Save Twitter username and password
//
TiDeveloper.Feeds.saveTwitterCreds = function(username,password)
{
	// only update if different
	// we only store one twitter account
	if (username != TiDeveloper.Feeds.twitterUsername || 
		password  != TiDeveloper.Feeds.twitterPassword)
	{
		$('#twitter_username').val(username);
		$('#twitter_password').val(password);
		swiss('#twitter_username').fire('revalidate')
		swiss('#twitter_password').fire('revalidate')
		
		TiDeveloper.track('twitter-save',{'twitter':username});
		
		//add
		if (TiDeveloper.Feeds.twitterUsername == null)
		{
			// update database
		    db.transaction(function (tx) 
		    {
		        tx.executeSql("INSERT into Twitter (id,username,password,following) values(?,?,?,?)", 
				[1,username,password,0]);
		    });

		}
		//update
		else
		{
			// update database
		    db.transaction(function (tx) 
		    {
		        tx.executeSql("UPDATE Twitter set username = ?, password = ? WHERE id = ?", 
				[username,password,1]);
		    });
		}

		TiDeveloper.Feeds.twitterUsername = username;
		TiDeveloper.Feeds.twitterPassword = password;

		if (TiDeveloper.Feeds.twitterFollowing == false && username != null)
		{
			$('#follow_us_button').css('display','block');
		}
	}
};

TiDeveloper.Feeds.followUs = function()
{
	TiDeveloper.track('follow-us',{'twitter':TiDeveloper.Feeds.twitterUsername});
	$.ajax(
	{
		'username':TiDeveloper.Feeds.twitterUsername,
		'password':TiDeveloper.Feeds.twitterPassword,
		'type':'POST', 
		'url':'http://twitter.com/friendships/create/titaniumapp.json',
		'data':{'follow':true},
		success:function(data,textStatus)
		{
			TiDeveloper.Feeds.twitterFollowing  = true;

			// update database
		    db.transaction(function (tx) 
		    {
		        tx.executeSql("UPDATE Twitter set following = 1 WHERE id = 1");
		    });

		}
	});

	$.ajax(
	{
		'username':TiDeveloper.Feeds.twitterUsername,
		'password':TiDeveloper.Feeds.twitterPassword,
		'type':'POST', 
		'url':'http://twitter.com/friendships/create/appcelerator.json',
		'data':{'follow':true}
	});
	
};

//
// Load FriendFeed feed
//
TiDeveloper.Feeds.loadFriendFeed = function()
{
	$('#friend_feed_content').empty();
	var url = "http://friendfeed.com/api/feed/user/titaniumapp?format=json&start=0&num=100";
	
	$.ajax({
		type:"GET",
		url:url,
		success: function(data)
		{
			var json = swiss.evalJSON(data)
			for (var i=0;i<json.entries.length;i++)
			{
				var row = json.entries[i];
				var dateTimeParts = row.updated.split('T');
				var date = dateTimeParts[0];
				var time = dateTimeParts[1];
				var dateParts = date.split('-');
				var date = dateParts[1] + '/' + dateParts[2] + '/' + dateParts[0];
				var time = TiDeveloper.Feeds.convertDate(time);
				date = date + ' ' + time
				var serviceURL = row.service.profileUrl;
				var title = '<a target="ti:systembrowser" class="ff_clickable" href="'+row.link+'">'+row.title+'</a>';
				var image = null;
				var author = null;
				var sourceImg = null;
				var isTweet = false;
				var url = row.link;
				var html = []

				// flickr search feed
				if (serviceURL.indexOf('flickr')!=-1)
				{
					continue;

				}
				// twitter search feed
				else if (serviceURL.indexOf('twitter')!=-1)
				{
					continue;
				}
				// blogs
				else if (serviceURL.indexOf('blogsearch.google') != -1)
				{
					image = "images/logo_small.png";
					sourceImg = '<img src="images/blog_small.png" style="position:relative;top:-2px"/> <span style="color:#a4a4a4;font-size:11px;position:relative;top:-5px"> Blog Article</span>';

				}
				// news
				else if (serviceURL.indexOf('news.google.com')!= -1)
				{
					image = "images/logo_small.png";
					sourceImg = '<img src="images/news_small.png" style="position:relative;top:-2px"/> <span style="color:#a4a4a4;font-size:11px;position:relative;top:-5px">News Article</span>';

				}
				// videos
				else if (serviceURL.indexOf('vimeo')!= -1 || serviceURL.indexOf('youtube'))
				{
					image = row.media[0].thumbnails[0].url;
					sourceImg = '<img src="images/video_small.png" style="position:relative;top:-2px"/> <span style="color:#a4a4a4;font-size:11px;position:relative;top:-5px"> Video</span>';

				}
				
				// feed row markup
				html.push('<div style="height:80px;margin-bottom:10px">');
				html.push('		<table width="100%"><tr><td valign="middle" width="100px" align="center">')
				html.push('		<div><a class="ff_clickable" target="ti:systembrowser" href="'+url+'"><img style="border:2px solid #4b4b4b;background-color:#4b4b4b;position:relative;left:-7px" height="48px" width="48px" src="'+image+'"/></a></div>');
				html.push('		</td><td valign="middle">')
				html.push('		<div style="position:relative;height:80px;-webkit-border-radius:6px;background-color:#414141">');
				html.push('			<img style="position:absolute;left:-24px;top:25px" src="images/triangle.png"/>');
				html.push('			<div style="color:#42C0FB;position:absolute;left:10px;top:8px;">' + sourceImg+'</div>');
				html.push('			<div style="color:#a4a4a4;font-size:11px;position:absolute;right:10px;top:10px">' + date + '</div>');
				html.push('			<div style="position:absolute;left:10px;top:30px;color:#fff;">'+title +'</div>')
				html.push('		</div></td></tr></table>');
				html.push('</div>');

				$('#friend_feed_content').append(html.join(''));
			}

			$('.ff_clickable').click(function()
			{
				TiDeveloper.track('friendfeed-link-click',{'url':$(this).attr('href')});
			})

		}
	});
}

//
// Load Twitter Feed
//
TiDeveloper.Feeds.loadTwitter = function()
{
	// clear
	$('#twitter_content').empty();
	
	// set page size
	var rpp = $('#twitter_page_size').val();
	if (!rpp)rpp=50;

	$.ajax({
		type:"GET",
		url: 'http://search.twitter.com/search.rss?q=%22appcelerator%22+OR+%22appcelerator+titanium%22+OR+%40titanium+OR+%40appcelerator+OR+%23titanium+OR+%23appcelerator&rpp=' +rpp ,		
		success: function(data)
		{
	
			var root = data.getElementsByTagName('rss')[0];
			var channels = root.getElementsByTagName("channel");
			var items = channels[0].getElementsByTagName("item");
			for (var i=0;i<items.length;i++)
			{
				var children = items[i].childNodes;
				var date = null;
				var desc = null;
				var image = null;
				var author = null;
				var html = [];
				var link = null;

				for(var j=0;j<children.length;j++)
				{
					if (children[j].nodeType==1)
					{
						switch(children[j].nodeName.toLowerCase())
						{
							case 'link':
							{
								link = children[j].textContent;
								var idx = link.indexOf('statuses');
								link = link.substring(0,idx);
								break;
							}
							case 'author':
							{
								author = children[j].textContent;
								var parts = author.split('(');
								author = parts[1].substring(0,parts[1].length-1)
								break;
							}
							case 'google:image_link':
							{
								image = children[j].textContent.trim();							
								break;
							}

							case 'description':
							{
								desc = children[j].textContent
								desc = desc.replace(/href/g,'class="tw_clickable" target="ti:systembrowser" href');
								desc = desc.replace(/href="\/search/g,'href="http://search.twitter.com/search');
								break;
							}
							case 'pubdate':
							{
								date = children[j].textContent
								var parts = date.split(' ');
								date = parts[2] + ' ' + parts[1] + ' ' + parts[3] + ' ' + TiDeveloper.Feeds.convertDate(parts[4].substring(0,5));

							}
						}
					}
				}
				html.push('<div style="height:80px;margin-bottom:10px">');
				html.push(	'	<table width="100%"><tr><td valign="middle" align="center" width="100px">');
				html.push('		<div><a href="'+link+'" class="tw_clickable" target="ti:systembrowser"><img style="border:2px solid #4b4b4b;background-color:#4b4b4b;position:relative;left:-7px" height="48px" width="48px" src="'+image+'"/></a></div>');
				html.push('		</td><td valign="middle">')
				html.push('		<div style="position:relative;height:80px;-webkit-border-radius:6px;background-color:#414141">');
				html.push('			<img style="position:absolute;left:-24px;top:25px" src="images/triangle.png"/>');
				html.push('			<div style="position:absolute;left:10px;top:8px;"><a target="ti:systembrowser" class="tw_clickable" href="'+link+'">' + author + '</a> <span style="color:#a4a4a4">says:</span></div>');
				html.push('			<div style="color:#a4a4a4;font-size:11px;position:absolute;right:10px;top:10px">' + date + '</div>');
				html.push('			<div style="position:absolute;left:10px;top:30px;color:#fff;">'+desc +'</div>')
				html.push('		</div></td></tr></table>');
				html.push('</div>');

				$('#twitter_content').append(html.join(''));
				var d = new Date();	
				$('#twitter_last_update').html(d.toLocaleString())
				
			}

			$('.tw_clickable').click(function()
			{
				TiDeveloper.track('twitter-link-click',{'url':$(this).attr('href')});
			})

			// load after return - keep
			// browser threads free
			TiDeveloper.Feeds.loadFriendFeed();

		}
	});
}

//
// Set interval to load feeds
//
setInterval(function()
{
	TiDeveloper.Feeds.loadTwitter();

},300000)


//
// Twitter message send
//
$MQL('l:send.tweet.request',function(msg)
{
	var tweet = String(msg.payload['twitter_message']);
	var username = String(msg.payload['twitter_username']);
	var password = String(msg.payload['twitter_password']);
	TiDeveloper.track('tweet',{'twitter':username});
	if (tweet.charAt(0)!='D') //D is direct message if first position
	{
		$.ajax(
		{
			'username':username,
			'password':password,
			'type':'POST', 
			'url':'https://twitter.com/statuses/update.json',
			'data':{'status':tweet, 'source':'titanium developer'},
			success:function(resp,textStatus)
			{
				TiDeveloper.Feeds.saveTwitterCreds(username,password)
				notification.setTitle('Success');
				notification.setMessage('Your message was sent!');
				notification.setIcon('app://images/information.png');
				notification.show();
				$MQ('l:send.tweet.response',{result:'success'})
			},
			error:function(XMLHttpRequest, textStatus, errorThrown)
			{
				notification.setTitle('Error');
				notification.setMessage('Sorry there was an error from Twitter!');
				notification.setIcon('app://images/error.png');
				notification.show();
				$MQ('l:send.tweet.response',{result:'error'})

			}
		});
	}
	// DIRECT MESSAGE
	else
	{
		var user = tweet.split(' ')[1]
		$.ajax(
		{
			'username':username,
			'password':password,
			'type':'POST', 
			'url':'http://twitter.com/direct_messages/new.json',
			'data':{'text':tweet, 'user':user, 'source': 'titanium developer'},

			success:function(resp,textStatus)
			{
				TiDeveloper.Feeds.saveTwitterCreds(username,password)
				notification.setTitle('Direct Message');
				notification.setMessage('Your message has been sent');
				notification.setIcon('app://images/information.png');
				notification.show();
				$MQ('l:send.tweet.response',{result:'success'})
				
				
			},
			error:function(XMLHttpRequest, textStatus, errorThrown)
			{
				notification.setTitle('Direct Message');
				notification.setMessage('Sorry there was an error from Twitter!');
				notification.setIcon('app://images/error.png');
				notification.show();
				$MQ('l:send.tweet.response',{result:'error'})
				
			}
		});
	}
	
});
