describe("Ti.Filesystem tests",{
	before_all:function()
	{
		// clean up testing folder if needed
		var base = Titanium.Filesystem.getFile(Titanium.Filesystem.getApplicationDataDirectory(), "unittest_filesystem");
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


	},
	
	filesystem_basic_static_properties: function()
	{
		var methods = ['getFile','getFileStream','getApplicationDataDirectory'];
		
		for (var c=0;c<methods.length;c++)
		{
			var method = methods[c];
			value_of(Titanium.Filesystem[method]).should_be_function();
		}
	},
	
	other_props:function()
	{
		value_of(Titanium.Filesystem.getLineEnding).should_be_function();
		value_of(Titanium.Filesystem.getSeparator).should_be_function();
		value_of(Titanium.Filesystem.MODE_READ).should_not_be_null();
		value_of(Titanium.Filesystem.MODE_WRITE).should_not_be_null();
		value_of(Titanium.Filesystem.MODE_APPEND).should_not_be_null();
				
		value_of(Titanium.Filesystem.getLineEnding()).should_not_be_null();
		value_of(Titanium.Filesystem.getSeparator()).should_not_be_null();
	},
	
	get_file: function()
	{
		var f = Titanium.Filesystem.getFile(this.base, "getFileTest.txt");
		value_of(f).should_not_be_null();
		// we only created a path to a file, not a real file.
		value_of(f.exists()).should_be_false();
	},
	
	get_file_stream:function()
	{
		var fs = Titanium.Filesystem.getFileStream(this.base, "getFileStreamTest.txt");
		value_of(fs).should_not_be_null();
	},
	
	temp_file:function()
	{
		value_of(Titanium.Filesystem.createTempFile).should_be_function();
		
		var f = Titanium.Filesystem.createTempFile();
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_true();
		value_of(f.isFile()).should_be_true();
		value_of(f.isDirectory()).should_be_false();
	},
	
	temp_directory:function()
	{
		value_of(Titanium.Filesystem.createTempDirectory).should_be_function();
		
		var f = Titanium.Filesystem.createTempDirectory();
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_true();	
		value_of(f.isFile()).should_be_false();
		value_of(f.isDirectory()).should_be_true();
	},
	
	common_directories:function()
	{
		value_of(Titanium.Filesystem.getProgramsDirectory).should_be_function();
		value_of(Titanium.Filesystem.getApplicationDirectory).should_be_function();
		value_of(Titanium.Filesystem.getRuntimeHomeDirectory).should_be_function();
		value_of(Titanium.Filesystem.getResourcesDirectory).should_be_function();
		value_of(Titanium.Filesystem.getDesktopDirectory).should_be_function();
		value_of(Titanium.Filesystem.getDocumentsDirectory).should_be_function();
		value_of(Titanium.Filesystem.getUserDirectory).should_be_function();
		
		value_of(Titanium.Filesystem.getProgramsDirectory()).should_not_be_null();
		value_of(Titanium.Filesystem.getApplicationDirectory()).should_not_be_null();
		value_of(Titanium.Filesystem.getRuntimeHomeDirectory()).should_not_be_null();
		value_of(Titanium.Filesystem.getResourcesDirectory()).should_not_be_null();
		value_of(Titanium.Filesystem.getDesktopDirectory()).should_not_be_null();
		value_of(Titanium.Filesystem.getDocumentsDirectory()).should_not_be_null();
		value_of(Titanium.Filesystem.getUserDirectory()).should_not_be_null();
	},
	
	test_ProgramsDirectory:function()
	{
		value_of(Titanium.Filesystem.getProgramsDirectory).should_be_function();
		value_of(Titanium.Filesystem.getProgramsDirectory()).should_not_be_null();
		
		var folder = Titanium.Filesystem.getProgramsDirectory();
		value_of(folder).should_not_be_null();
		value_of(folder.exists()).should_be_true();	
		value_of(folder.isFile()).should_be_false();
		value_of(folder.isDirectory()).should_be_true();
		
		Titanium.API.debug("Titanium.Filesystem.getProgramsDirectory="+folder.nativePath());

		var progFileFolder = null;
				
		if ( Titanium.platform == 'win32' )
		{
			// in windows we have an environment variable we can check
			// for the program files directory.
			
			progFileFolder = Titanium.API.getEnvironment()["PROGRAMFILES"];
			Titanium.API.debug("windows env PROGRAMFILES="+progFileFolder);
		}
		else if ( Titanium.platform == 'osx' )
		{	
			// on the Mac, this should be hard coded to '/Applications' according to the source code.
			progFileFolder = "/Applications";
		}
		else if (Titanium.platform == 'linux' )
		{
			// currently under linux we are hard coded to this path.  this could change
			progFileFolder = "/usr/local/bin";
		}
		else 
		{
			failed("unknown platform");
		}
		value_of(progFileFolder.indexOf(folder.nativePath())).should_not_be(-1);
	},
	
	test_ApplicationDirectory: function()
	{
		value_of(Titanium.Filesystem.getApplicationDirectory).should_be_function();
		value_of(Titanium.Filesystem.getApplicationDirectory()).should_not_be_null();
		
		var f = Titanium.Filesystem.getApplicationDirectory();
		Titanium.API.debug("application folder"+f.nativePath());
		value_of( f == Titanium.App.home ).should_be_true();		
	},
	
	test_RuntimeHomeDirectory: function()
	{
		value_of(Titanium.Filesystem.getRuntimeHomeDirectory).should_be_function();
		value_of(Titanium.Filesystem.getRuntimeHomeDirectory()).should_not_be_null();

		var f = Titanium.Filesystem.getRuntimeHomeDirectory();
		Titanium.API.debug("runtime home folder"+f.nativePath());
		
		var componentPaths = Titanium.API.getComponentSearchPaths()
		value_of(componentPaths).should_be_array();
		
		var bfound = false;
		
		for (i=0; i< componentPaths.length; i++ )		
		{
			if ( f == componentPaths[i] )
			{
				Titanium.API.debug("runtime reported path = "+componentPaths[i].toString());
				bfound = true;
				break;
			}
		}
		value_of( bfound ).should_be_true();
	},
	
	test_ResourcesDirectory: function()
	{
		value_of(Titanium.Filesystem.getResourcesDirectory).should_be_function();
		value_of(Titanium.Filesystem.getResourcesDirectory()).should_not_be_null();

		var f = Titanium.Filesystem.getResourcesDirectory();
		Titanium.API.debug("Resources folder"+f.nativePath());
		
		value_of( f.nativePath().indexOf(Titanium.Filesystem.getApplicationDirectory())).should_not_be(-1);
	},
	test_DesktopDirectory: function()
	{
		value_of(Titanium.Filesystem.getDesktopDirectory).should_be_function();
		value_of(Titanium.Filesystem.getDesktopDirectory()).should_not_be_null();

		var desktop = Titanium.Filesystem.getDesktopDirectory();
		value_of(desktop).should_not_be_null();

		var userHome = null;		
		// we have an environment variable we can check
		// for the user home directory.  then we can make sure it's a substring
		// of the desktop folder path
		if ( Titanium.platform == 'win32' )
		{
			userHome = Titanium.API.getEnvironment()["USERPROFILE"];
			Titanium.API.debug(Titanium.platform+" env USERPROFILE="+userHome);
		}
		else 
		{	
			// on the Mac, this should be hard coded to '~/desktop' according to the source code.
			// on linux, this will be hard coded in a similar fashion
			userHome = Titanium.API.getEnvironment()["HOME"];
			Titanium.API.debug(Titanium.platform+" env HOME="+userHome);
		}
		var path = desktop.nativePath();
		value_of(path.indexOf(userHome)).should_not_be(-1);
	},
	test_DocumentsDirectory: function()
	{
		value_of(Titanium.Filesystem.getDocumentsDirectory).should_be_function();
		value_of(Titanium.Filesystem.getDocumentsDirectory()).should_not_be_null();
		
		var documents = Titanium.Filesystem.getDesktopDirectory();
		value_of(documents).should_not_be_null();

		var userHome = null;		
		// we have an environment variable we can check
		// for the user home directory.  then we can make sure it's a substring
		// of the desktop folder path
		if ( Titanium.platform == 'win32' )
		{
			userHome = Titanium.API.getEnvironment()["USERPROFILE"];
			Titanium.API.debug(Titanium.platform+" env USERPROFILE="+userHome);
		}
		else 
		{	
			// on the Mac, this should be hard coded to '~/desktop' according to the source code.
			// on linux, this will be hard coded in a similar fashion
			userHome = Titanium.API.getEnvironment()["HOME"];
			Titanium.API.debug(Titanium.platform+" env HOME="+userHome);
		}
		var path = documents.nativePath();
		value_of(path.indexOf(userHome)).should_not_be(-1);
	},
	test_UserDirectory: function()
	{
		value_of(Titanium.Filesystem.getUserDirectory).should_be_function();
		value_of(Titanium.Filesystem.getUserDirectory()).should_not_be_null();
		
		var f = Titanium.Filesystem.getUserDirectory();
		value_of(f).should_not_be_null();
		Titanium.API.debug("user home folder ="+f.nativePath());

		var userHome = null;		
		if ( Titanium.platform == 'win32'  ) 
		{
			// in windows we have an environment variable we can check
			// for the program files directory.
			userHome = Titanium.API.getEnvironment()["USERPROFILE"];
			Titanium.API.debug(Titanium.platform +" environment USERPROFILE="+userHome);
		}
		else
		{	
			// on the Mac, this should be hard coded to '~/desktop' according to the source code.
			userHome = Titanium.API.getEnvironment()["HOME"];
			Titanium.API.debug(Titanium.platform +" environment HOME="+userHome);
		}
		value_of(f.nativePath()).should_be(userHome);
	},
	
	root_directories:function()
	{
		value_of(Titanium.Filesystem.getRootDirectories).should_be_function();
		
		var rootDirs = Titanium.Filesystem.getRootDirectories();
		value_of(rootDirs).should_not_be_null();
		value_of(rootDirs.length>0).should_be_true();
		var rootDirFirst = rootDirs[0];
		value_of(rootDirFirst).should_be_object();
		try {
			value_of(rootDirFirst.isDirectory()).should_be_true();
		} catch (e) {
			// swallow accesss errors here, silly windows
			if (e.toString().indexOf("File access error") == -1) {
				throw e;
			}
		}
	},
	
	async_copy:function()
	{
		var fromDir = Titanium.Filesystem.getFile(this.base, "ayncCopyFrom");
		var toDir = Titanium.Filesystem.getFile(this.base, "asynCopyTo");
		this.createDirTree(this.base,"ayncCopyFrom");
		Titanium.Filesystem.asyncCopy(fromDir,toDir,function() {
			
			value_of(AsyncCopy.running).should_be_true();
			
			var listings = toDir.getDirectoryListing();
			value_of(listings).should_not_be_null();
			value_of(listings.length==3).should_be_true();
			
			var toSubDir1 = Titanium.Filesystem.getFile(fromDir, "subDir1");
			value_of(toSubDir1.isDirectory()).should_be_true();
			
			var subDirListings = toSubDir1.getDirectoryListing();
			value_of(subDirListings).should_not_be_null();
			value_of(subDirListings.length==1).should_be_true();
		});	
	},
	
	test_line_endings: function()
	{
		value_of(Titanium.Filesystem.getLineEnding).should_be_function();
		value_of(Titanium.Filesystem.getLineEnding()).should_not_be_null();
		
        if ( Titanium.platform == 'win32' )
        {
            // this is weird, we need to investigate further.
            value_of(Titanium.Filesystem.getLineEnding()).should_be("\n");
        }
        else 
        {
            value_of(Titanium.Filesystem.getLineEnding()).should_be("\n");
        }
	},
	
	test_separator: function ()
	{
		value_of(Titanium.Filesystem.getSeparator).should_be_function();
		value_of(Titanium.Filesystem.getSeparator()).should_not_be_null();
		
        if ( Titanium.platform == 'win32' )
        {
            value_of(Titanium.Filesystem.getSeparator()).should_be("\\");
        }
        else 
        {
            value_of(Titanium.Filesystem.getSeparator()).should_be("/");
        }
	}
});

