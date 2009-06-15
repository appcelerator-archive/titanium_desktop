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
	
	get_file:function()
	{
		var f = Titanium.Filesystem.getFile(this.base, "getFileTest.txt");
		value_of(f).should_not_be_null();
	},
	
	get_file_stream:function()
	{
		var fs = Titanium.Filesystem.getFile(this.base, "getFileStreamTest.txt");
		value_of(fs).should_not_be_null();
	},
	
	temp_file:function()
	{
		value_of(Titanium.Filesystem.createTempFile).should_be_function();
		
		var f = Titanium.Filesystem.createTempFile();
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_true();
	},
	
	temp_directory:function()
	{
		value_of(Titanium.Filesystem.createTempDirectory).should_be_function();
		
		var f = Titanium.Filesystem.createTempDirectory();
		value_of(f).should_not_be_null();
		value_of(f.exists()).should_be_true();	
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
	
	root_directories:function()
	{
		value_of(Titanium.Filesystem.getRootDirectories).should_be_function();
		
		var rootDirs = Titanium.Filesystem.getRootDirectories();
		value_of(rootDirs).should_not_be_null();
		value_of(rootDirs.length>0).should_be_true();
		var rootDirFirst = rootDirs[0];
		value_of(rootDirFirst).should_be_object();
		value_of(rootDirFirst.isDirectory()).should_be_true();	
	},
	
	async_copy:function()
	{
		var fromDir = Titanium.Filesystem.getFile(this.base, "ayncCopyFrom");
		var toDir = Titanium.Filesystem.getFile(this.base, "asynCopyTo");
		
		this.createDirTree(this.base,"ayncCopyFrom");
		Titanium.Filesystem.asyncCopy(fromDir,toDir,function() {
			var listings = toDir.getDirectoryListing();
			value_of(listings).should_not_be_null();
			value_of(listings.length==3).should_be_true();
			
			var toSubDir1 = Titanium.Filesystem.getFile(fromDir, "subDir1");
			value_of(toSubDir1.isDirectory()).should_be_true();
			
			var subDirListings = toSubDir1.getDirectoryListing();
			value_of(subDirListings).should_not_be_null();
			value_of(subDirListings.length==1).should_be_true();
		});	
	}
});

