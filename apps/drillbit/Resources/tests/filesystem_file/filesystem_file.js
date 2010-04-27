describe("Ti.Filesystem File tests",
{
	before_all:function()
	{
		// clean up testing folder if needed
		var base = Titanium.Filesystem.getFile(Titanium.Filesystem.getApplicationDataDirectory(), "unittest_filesystem_file");
		if(base.exists() && base.isDirectory()) {
			base.deleteDirectory(true);
		}
		else if(base.exists() && base.isFile()) {
			base.deleteFile();
		}
	
		base.createDirectory();
		
		this.base = base;

		this.createDirTree = function(base,name) {
			var dir = Titanium.Filesystem.getFile(base, name);
			if(! dir.exists()) {
				dir.createDirectory();
			}

			var file1 = Titanium.Filesystem.getFileStream(dir, "file1.txt");
			var file2 = Titanium.Filesystem.getFileStream(dir, "file2.txt");
			var subDir1 = Titanium.Filesystem.getFile(dir, "subDir1");
			subDir1.createDirectory();
			var file3 = Titanium.Filesystem.getFileStream(subDir1, "file3.txt");

			file1.open(Titanium.Filesystem.MODE_WRITE);
			file1.write("Text for file1");
			file1.close();

			file2.open(Titanium.Filesystem.MODE_WRITE);
			file2.write("Text for file2");
			file2.close();

			file3.open(Titanium.Filesystem.MODE_WRITE);
			file3.write("Text for file3");
			file3.close();
		};
		
		this.createFile = function(base,name,text) {
			var fs = Titanium.Filesystem.getFileStream(base, name);
			fs.open(Titanium.Filesystem.MODE_WRITE);
			fs.write(text);
			fs.close();
		};

	},
	
	file_props:function()
	{
		var f = Titanium.Filesystem.getFile(this.base, "newFile.txt");
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_false();
		
		// create new file to test props with
		this.createFile(this.base,"filePropsTest.txt", "This is the text for the text file.");
		
		f = Titanium.Filesystem.getFile(this.base, "filePropsTest.txt");
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_true();
		value_of(f.toString()).should_not_be_null();
		value_of(f.name()).should_not_be_null();
		value_of(f.extension()).should_be('txt');
		value_of(f.isFile()).should_be_true();
		value_of(f.isDirectory()).should_be_false();
		value_of(f.isHidden()).should_be_false();
		value_of(f.isSymbolicLink()).should_be_false();
		value_of(f.isExecutable()).should_be_false();
		value_of(f.isReadonly()).should_be_false();
		value_of(f.isWriteable()).should_be_true();
		value_of(f.createTimestamp()).should_not_be_null();
		value_of(f.modificationTimestamp()).should_not_be_null();
		value_of(f.nativePath()).should_not_be_null();
		value_of(f.size()).should_not_be_null();	
		
		// this should throw exception
		try
		{
			f.write(null);
			failed('should have failed on null write');
		}
		catch(e)
		{
			//good
		}
	},
	
	directory_props:function()
	{
		// directory props
		var f = Titanium.Filesystem.getFile(this.base, "dirPropsTest");
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_false();
		f.createDirectory();
		value_of(f.exists()).should_be_true();
		value_of(f.isHidden()).should_be_false();
		value_of(f.isSymbolicLink()).should_be_false();
		// directories are by default executable
		// -- in win32 this impl only checks to see if this is an .exe (so skip)
		if (Titanium.platform != "win32") {
			value_of(f.isExecutable()).should_be_true();
		}

		value_of(f.isReadonly()).should_be_false();
		value_of(f.isWriteable()).should_be_true();
		value_of(f.createTimestamp()).should_not_be_null();
		value_of(f.modificationTimestamp()).should_not_be_null();
		value_of(f.nativePath()).should_not_be_null();
		value_of(f.size()).should_not_be_null();
		value_of(f.spaceAvailable()).should_not_be_null();
	},
	
	resolve:function()
	{
		var f = f = Titanium.Filesystem.getFile(this.base, "fileTest-Resolve");
		value_of(f).should_not_be_null();
		value_of(f.resolve("filename.txt")).should_not_be_null();
	},
	
	file_operations:function()
	{
		var f = Titanium.Filesystem.getFile(this.base, "fileToCopy.txt");
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_false();
		
		// create new file to test props with
		this.createFile(this.base,"fileToCopy.txt","This ist he text for the test file.");
		
		var copiedF = Titanium.Filesystem.getFile(this.base, "copiedFile.txt");
		var r = f.copy(copiedF);
		value_of(r).should_be_true();
		value_of(copiedF.exists()).should_be_true();
		
		var movedF = Titanium.Filesystem.getFile(this.base, "movedFile.txt");
		r = copiedF.move(movedF);
		value_of(r).should_be_true();
		value_of(movedF.exists()).should_be_true();
		
		var renamedF = Titanium.Filesystem.getFile(this.base, "renamedFile.txt");
		r = movedF.rename("renamedFile.txt");
		value_of(r).should_be_true();
		value_of(renamedF.exists()).should_be_true();
		
		r = renamedF.deleteFile();
		value_of(r).should_be_true();

		// touch to create a new file
		var t = Titanium.Filesystem.getFile(this.base, "touched.txt");
		value_of(t.exists()).should_be_false();
		value_of(t.touch()).should_be_true();
		value_of(t.exists()).should_be_true();
		value_of(t.touch()).should_be_false();
	},
	
	directory_operations:function()
	{
		var d = Titanium.Filesystem.getFile(this.base, "playDirectory");
		value_of(d).should_not_be_null();
		value_of(d.exists()).should_be_false();
		
		// create new directory
		var r = d.createDirectory();
		value_of(r).should_be_true();
		value_of(d.exists()).should_be_true();
		
		r = d.deleteDirectory();
		value_of(r).should_be_true();
		value_of(d.exists()).should_be_false();
	},
	
	directory_listing:function()
	{
		var d = Titanium.Filesystem.getFile(this.base, "directoryListingTest");
		value_of(d).should_not_be_null();
		value_of(d.exists()).should_be_false();
		
		this.createDirTree(this.base,"directoryListingTest");
		value_of(d.exists()).should_be_true();

		var listings = d.getDirectoryListing();
		value_of(listings).should_not_be_null();
		value_of(listings.length).should_be(3);
		
		var subDir1 = Titanium.Filesystem.getFile(d, "subDir1");
		value_of(subDir1.isDirectory()).should_be_true();
		
		var subDirListings = subDir1.getDirectoryListing();
		value_of(subDirListings).should_not_be_null();
		value_of(subDirListings.length).should_be(1);
	},
	
	parent:function()
	{
		var f = Titanium.Filesystem.getFile(this.base, "parentTestFile.txt");
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_false();
		
		var parent = f.parent();
		value_of(parent.toString()).should_be(this.base.toString());
	},
	
	shortcut:function()
	{
		this.createFile(this.base,"shortcutTestFile.txt","text for test file");
		var f = Titanium.Filesystem.getFile(this.base, "shortcutTestFile.txt");
		
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_true();
		
		// use .lnk in win32
		var shortcutFilename = "my-shortcut";
		if (Titanium.platform == "win32") {
			shortcutFilename += ".lnk";
		}
		
		var shortcutFile = Titanium.Filesystem.getFile(this.base, shortcutFilename);
		var r = f.createShortcut(shortcutFile);
		value_of(r).should_be_true();
		value_of(shortcutFile.exists()).should_be_true();
	},
	
	file_permissions:function()
	{
		this.createFile(this.base,"permissionsTestFile.txt","text for test file");
		var f = Titanium.Filesystem.getFile(this.base, "permissionsTestFile.txt");
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_true();
		value_of(f.isExecutable()).should_be_false();
		value_of(f.isReadonly()).should_be_false();
		value_of(f.isWriteable()).should_be_true();
		
		if (Titanium.platform != "win32") {
			// POCO doesn't implement executable for win32
			f.setExecutable(true);
			value_of(f.isExecutable()).should_be_true();

			f.setExecutable(false);
			value_of(f.isExecutable()).should_be_false();
		}

		f.setReadonly(false);
		value_of(f.isReadonly()).should_be_false();

		f.setReadonly(true);
		value_of(f.isReadonly()).should_be_true();
		
		f.setWriteable(false);
		value_of(f.isWriteable()).should_be_false();

		f.setWriteable(true);
		value_of(f.isWriteable()).should_be_true();
	},
	
	test_file_readLine_isEmpty:function()
	{
		this.createFile(this.base,"readline.txt","\nfoo\n\n");
		var f = Titanium.Filesystem.getFile(this.base, "readline.txt");
		value_of(f).should_not_be_null();
		var c=0;
		while(c<5)
		{
			var line = f.readLine(c==0 ? true : false);
			switch(c)
			{
				case 0:
				case 2:
					value_of(line).should_be('');
					break;
				case 1:
					value_of(line).should_be('foo');
					break;
				case 3:
					value_of(line).should_be_null();
					break;
			}
			c++;
			if (line==null) break;
		}
		value_of(c).should_be(4);
	},
	
	test_file_create_timestamp: function()
	{
		var curDate = new Date();
		
		this.createFile(this.base,"timestamp.dat", curDate.toUTCString());
		
		var f = Titanium.Filesystem.getFile(this.base, "timestamp.dat");
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_true();
		value_of(f.createTimestamp()).should_not_be_null();

		var timestamp = f.createTimestamp();
		var tsDate = new Date();
		
		tsDate.setTime(timestamp);
		
		Titanium.API.debug("date "+curDate+ " " +tsDate);
		value_of(curDate < tsDate ).should_be_true();
		
		value_of(f.deleteFile()).should_be_true();
	},
	
	test_file_modify_timestamp: function()
	{
		var curDate = new Date();
		
		this.createFile(this.base,"modifydate.dat", curDate.toUTCString());
		
		var f = Titanium.Filesystem.getFile(this.base, "modifydate.dat");
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_true();
		value_of(f.createTimestamp()).should_not_be_null();
		value_of(f.modificationTimestamp()).should_not_be_null();

		var timestamp = f.createTimestamp();
		var tsDate = new Date();
		
		tsDate.setTime(timestamp);
		
		Titanium.API.debug("date "+curDate+ " " +tsDate);
		value_of(curDate < tsDate ).should_be_true();
		
		if ( !f.isWriteable() )
		{
			f.setWriteable();
			value_of(f.isWriteable()).should_be_true();
		}
		
		var modDate = new Date();
		// write some stuff to the file
		f.write(modDate.toUTCString());
		
		// get the modification date
		timestamp = f.modificationTimestamp();
		modDate.setTime(timestamp);
		value_of(tsDate <= modDate).should_be_true();
		value_of(f.deleteFile()).should_be_true();
	},
	
	test_available_space: function()
	{
		// retrieve a known folder so we can calculate the disk space
		var dir = Titanium.Filesystem.getProgramsDirectory().nativePath();
		var f = Titanium.Filesystem.getFile(dir);
		value_of(f).should_not_be_null();
		
		value_of(f.spaceAvailable()).should_be_number();
		value_of(f.spaceAvailable()).should_not_be(0);
	},
	test_read_large_file: function()
	{
		var data = make_large_file();
		var file = Titanium.Filesystem.getFile(data[0]);
		var contents = file.read();
		file.deleteFile();
		value_of(contents.length).should_be(data[1].length);
		value_of(String(contents)).should_be(data[1]);
	},
	test_file_url_three_slashes: function()
	{
		// File URLs on all platforms needs to begin with three slashes
		// (effectively an empty host name)
		var file = Titanium.Filesystem.getFile(
			Titanium.API.application.dataPath, "a_file.txt");
		value_of(file.toURL().indexOf("file:///")).should_be(0);
	},
});

