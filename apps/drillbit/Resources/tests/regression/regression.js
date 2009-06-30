describe('Regression tests', {
	// Lighthouse issues
	issue32_unicode_in_title: function()
	{
		var win = Titanium.UI.getCurrentWindow().createWindow('http://en.wikipedia.org/wiki/The_Evil_Dead');
		win.setTitle("'添加新帐户");
		value_of(win.getTitle()).should_be("'添加新帐户");
	},
	
	issue35_open_sound_crashes_as_async: function(callback)
	{
		var sound = Titanium.Media.createSound('app://sound.wav');
		sound.play();
		
		var timer = setTimeout(function(){
			window.location.href = 'http://en.wikipedia.org/wiki/The_Evil_Dead';
			callback.passed();
		}, 3000);
	},
	
	// old JIRA issues for regression
	TI328_open_and_write_filestream: function()
	{
		var filename = ".data.txt";
		var contents = "this is some data";
		var userDir = Titanium.Filesystem.getUserDirectory();
		var sessionFile = Titanium.Filesystem.getFile(userDir, filename);
		var sessionStream = Titanium.Filesystem.getFileStream(sessionFile);
		sessionStream.open(Titanium.Filesystem.MODE_WRITE);
		sessionStream.write(contents);
		sessionStream.close();
		
		value_of(sessionFile.exists()).should_be_true();
		value_of(sessionFile.size()).should_be(contents.length);
		sessionFile.deleteFile();
		value_of(sessionFile.exists()).should_be_false();
	},
	
	TI303_useragent_with_safari: function()
	{
		value_of(navigator.userAgent.indexOf('Safari') >= 0).should_be_true();
	},
	
	TI224_test_mixcase_urls: function()
	{
		var url = 'http://en.wikipedia.org/wiki/The_Evil_Dead';
		var w = Titanium.UI.createWindow(url);
		w.open();
		
		value_of(w.getURL()).should_be(url);
		w.close();
	}
});