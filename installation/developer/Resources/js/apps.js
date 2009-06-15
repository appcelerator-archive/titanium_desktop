TiDeveloper.Apps = {};
TiDeveloper.Apps.app_list_url = 'http://publisher.titaniumapp.com/api/app-list';
TiDeveloper.Apps.app_rate_url = 'http://publisher.titaniumapp.com/api/app-rate';

//
// Rating Function
//
TiDeveloper.Apps.setupRating = function(data)
{
	// set rating
	for (var i=0;i<data.length;i++)
	{
		var value = data[i].value;
		var id = data[i]['app_id'];
		for (var j=1;j<=value;j++)
		{
			$('#rating_' + id + '_' + j).removeClass('rating_off');
			
		}
	}
	
	// capture rating on click
	$('.rating').click(function()
	{
		var id = $(this).attr('app_id');
		var guid = $(this).attr('guid');
		var ratingStr = $('#rating_string_' + id);
		var hasVoted = ratingStr.attr('hasVoted');
		var vote = parseInt($(this).attr('star'));
		var votes = parseInt(ratingStr.attr('votes'));
		var rating = parseFloat(ratingStr.attr('rating'));
		
		
		// we only count vote
		if (hasVoted == "true")
		{
			ratingStr.css('display','none');
			$('#already_voted_'+id).fadeIn();
			setTimeout(function()
			{
				$('#already_voted_'+id).css('display','none');
				ratingStr.fadeIn();
				
			},1500);
		}
		else
		{
			var totalRating = (votes * rating) + vote;
			votes++;
			var newRating = String(totalRating/votes);
			if (newRating.indexOf('.')!=-1)
			{
				newRating = newRating.split('.')
				newRating = newRating[0] + "." + newRating[1].substring(0,2);
			}
			else
			{
				newRating = newRating;
			}

			ratingStr.css('display','none');
			ratingStr.html(newRating+' rating from '+votes+' votes');
			$('#voted_success_'+id).fadeIn();
			setTimeout(function()
			{
				$('#voted_success_'+id).css('display','none');
				ratingStr.fadeIn();
				
			},1500);

			// do we need to update stars
			if (parseFloat(newRating) > (parseInt(rating) + 1))
			{
				$('#rating_' + id + '_' + (parseInt(rating) +1)).removeClass('rating_off');
			}
			else if (parseFloat(newRating) < parseInt(rating))
			{
				$('#rating_' + id + '_' + rating).addClass('rating_off');
			}
			ratingStr.attr('rating',newRating);
			ratingStr.attr('hasVoted','true');
			ratingStr.attr('votes',votes);
			
			//TODO NOTE: rating should be 1-5
			var url = TiDeveloper.make_url(TiDeveloper.Apps.app_rate_url,{
				'rating':vote,
				'guid':guid,
				'mid':Titanium.Platform.id
			});
			
			// submit the new rating
			$.get(url);
			
			TiDeveloper.track('app-rate',{guid:guid,rating:vote});
		}
	});
	

	// enable mouseover
	$('.rating').mouseover(function()
	{
		var id = $(this).attr('app_id');
		var star = parseInt($(this).attr('star'));
		
		// start 3
		// rating 5
		for (var i=star;i>=1;i--)
		{
			$('#rating_' + id + '_' + i).removeClass('rating_off');
		}
		for (var i=5;i>star;i--)
		{
			$('#rating_' + id + '_' + i).addClass('rating_off');
		}
	});

	// enable mouseout
	$('.rating').mouseout(function()
	{
		var id = $(this).attr('app_id');
		var rating = parseInt($('#rating_string_'+id).attr('rating'));
		rating++;
		for (var i=rating;i<=5;i++)
		{
			$('#rating_' + id + '_' + i).addClass('rating_off');
		}
		for (var i=1;i<rating;i++)
		{
			$('#rating_' + id + '_' + i).removeClass('rating_off');
		}
	});
}

//
// listen for app search results
// and format star ratings
//
$MQL('l:applist',function(msg)
{
	// need delay to let rows get into DOM
	setTimeout(function()
	{
		TiDeveloper.Apps.setupRating(msg.payload['rows']);
		
	},100)
})

//
// Mock Service
//
$MQL('l:menu',function(msg)
{
	if (msg.payload.val == 'apps')
	{
		var url = TiDeveloper.make_url(TiDeveloper.Apps.app_list_url,{
			'f':'json',
			'mid':Titanium.Platform.id
		});
		
		$.getJSON(url,function(result)
		{
			TiDeveloper.Apps.formatAppRows(result);
		});
	}
});

TiDeveloper.Apps.findApps = function(options)
{
	var url = TiDeveloper.make_url(TiDeveloper.Apps.app_list_url,{
		'f':'json',
		'mid':Titanium.Platform.id,
		'o':options.sort,
		'q':options.search
	});
	
	$.getJSON(url,function(result)
	{
		TiDeveloper.Apps.formatAppRows(result);
	});
	
};

//
// Search Call
//
$MQL('l:app_search',function(msg)
{
	var sort = ($('#sort_most_downloaded').hasClass('active_sort'))?'downloaded':'popular';
	var options = {sort:sort,search:$('#app_search').val()};
	TiDeveloper.Apps.findApps(options);
	TiDeveloper.track('app-search',{search:$('#app_search').val()});
	
})

//
// Sort Call
//
$MQL('l:toggle_app_sort',function(msg)
{
	var sort = 'popular'
	if (msg.payload.val == 'download')
	{
		sort = 'download'
	}
	var options = {sort:sort,search:$('#app_search').val()};
	TiDeveloper.Apps.findApps(options);

});
TiDeveloper.Apps.formatAppRows = function(json)
{
	var count = json.length;
	var a = [];
	for (var i=0;i<json.length;i++)
	{
		var row = json[i];
		var title = (row.title.length > 18)?row.title.substring(0,17)+'...':row.title;
		var pubdate = TiDeveloper.Projects.formatPackagingDate(row.pubdate);
		var desc = row.desc;
		var url = row.url;
		var author = row.author;
		var downloads = row.downloads;
		var value = row.value;
		var votes = row.votes;
		var hasVoted = row.hasVoted;
		var link = row.download_url;
		var guid = row.guid;
		var image = row.image;
		var app_id = row.id
		var app_page = row.app_page
		a.push({'app_page':app_page,image:image,app_id:app_id,guid:guid,title:title,pubdate:pubdate,desc:desc,url:url,author:author,downloads:downloads,value:value,votes:votes,hasVoted:hasVoted,link:link})
		
	}
	if (count == 0)
	{
		$('#app_search_string').html('No Apps found.  Please try your search again.')
	}
	else
	{
		var plural = (count == 1)?'app':'apps';
		var sort = ($('#sort_most_downloaded').hasClass('active_sort'))?'most downloaded':'most popular';
		$('#app_search_string').html('We found ' + count + ' ' + plural + ' (sorted by ' + sort + ')');
	}
	$MQ('l:applist',{'rows':a,count:count});
	
};

$MQL('l:track_app_download',function(msg)
{
	TiDeveloper.track('app-download-click',{url:msg.payload.val});
	
})
$MQL('l:track_app_author',function(msg)
{
	TiDeveloper.track('app-website-click',{url:msg.payload.val});
	
});
$MQL('l:track_app_all',function(msg)
{
	TiDeveloper.track('app-weblinks-click',{url:msg.payload.val});
	
});