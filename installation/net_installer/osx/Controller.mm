/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#import "Controller.h"
#import <string>
#import <sys/mount.h>

#define RUNTIME_UUID_FRAGMENT @"uuid="RUNTIME_UUID
#define MODULE_UUID_FRAGMENT @"uuid="MODULE_UUID
#define SDK_UUID_FRAGMENT @"uuid="SDK_UUID
#define MOBILESDK_UUID_FRAGMENT @"uuid="MOBILESDK_UUID

@implementation Job
static int totalDownloads = 0;
static int totalJobs = 0;

-(void)dealloc
{
	[url release];
	[path release];
	[super dealloc];
}

-(Job*)initUpdate:(NSString*)pathOrURL
{
	self = [self init:pathOrURL];
	isUpdate = YES;
	return self;
}

-(Job*)init:(NSString*)pathOrURL
{
	self = [super init];

	NSFileManager *fm = [NSFileManager defaultManager];
	if ([fm fileExistsAtPath:pathOrURL])
	{
		url = nil;
		path = pathOrURL;
		[path retain];
	}
	else
	{
		url = [NSURL URLWithString:pathOrURL];
		[url retain];
		path = nil;
		totalDownloads++;
	}
	isUpdate = NO;
	totalJobs++;
	return self;
}

-(NSURL*)url
{
	return url;
}

-(NSString*)path
{
	return path;
}

-(void)setPath:(NSString*)newPath
{
	if (path != nil)
		[path release];
	path = newPath;
	[path retain];
}

-(BOOL)needsDownload
{
	return path == nil || isUpdate;
}

-(BOOL)isUpdate;
{
	return isUpdate;
}

-(int)totalDownloads;
{
	return totalDownloads;
}
-(int)totalJobs;
{
	return totalJobs;
}
@end

@implementation Controller

-(void)dealloc
{
	[jobs release];
	[installDirectory release];

	if (temporaryDirectory)
	{
		[[NSFileManager defaultManager] 
			removeFileAtPath:temporaryDirectory handler:nil];
		[temporaryDirectory release];
	}

	if (updateFile)
		[updateFile release];

	[super dealloc];
}


-(NSProgressIndicator*)progressBar
{
	return progressBar;
}

-(void)updateMessage:(NSString*)msg
{
	[progressText setStringValue:msg];
}


-(NSString*)temporaryDirectory
{
	return temporaryDirectory;
}

-(NSString*)installDirectory
{
	return installDirectory;
}

-(void)bailWithMessage:(NSString*)errorString;
{
	NSLog(@"Bailing with error: %@", errorString);
	NSRunCriticalAlertPanel(@"Error", errorString, @"Cancel", nil, nil);
	exit(1);
}

-(void)createDirectory:(NSString*)path;
{
	NSFileManager* fm = [NSFileManager defaultManager];
	NSError* error = nil;
	BOOL isDirectory;
	BOOL exists = [fm fileExistsAtPath:path isDirectory:&isDirectory];

	if (!exists)
	{
		[fm createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:&error];
	}
	else if (!isDirectory)
	{
		NSString* msg = [NSString stringWithFormat:@"Installer tried to create the folder \"%@\", but found a file in its place.", path];
		[self performSelectorOnMainThread:@selector(bailWithMessage:) withObject:msg waitUntilDone:YES];
	}

	if (error != nil)
	{
		NSString* msg = [NSString stringWithFormat:@"Installer tried to create the folder \"%@\", but encountered error: %@", path, error];
		[self performSelectorOnMainThread:@selector(bailWithMessage:) withObject:msg waitUntilDone:YES];
	}
}

-(void)install:(Job*)job 
{
	NSString* name = @"unknown";
	NSString* version = @"unknown";
	KComponentType type = KrollUtils::UNKNOWN;

	NSString* path = [job path];
	NSURL* url = [job url];
	if ([job isUpdate])
	{
		type = KrollUtils::APP_UPDATE;
		name = @"update";
		version = [NSString stringWithUTF8String:app->version.c_str()];
	}
	else if (url == nil)
	{
		// The file is either in the format of module-modname-version.zip for a
		// module or runtime-version.zip for the runtime, so we need to split
		// on '-' and count the parts to figure out what it is.
		NSArray* fileParts = [path componentsSeparatedByString:@"/"];
		NSString* trimmed = [[fileParts lastObject] stringByDeletingPathExtension];
		NSArray* parts = [trimmed componentsSeparatedByString:@"-"];

		NSString* typeString = [parts objectAtIndex:0];
		typeString = [typeString lowercaseString];
		if ([typeString isEqualToString:@"module"] && (int) [parts count] >= 3)
		{
			// part 0 should be "module"
			type = KrollUtils::MODULE;
			name = [parts objectAtIndex:1];
			version = [parts objectAtIndex:2];
		}
		else if ([typeString isEqualToString:@"runtime"])
		{
			type = KrollUtils::RUNTIME;
			name = [parts objectAtIndex:0];
			version = [parts objectAtIndex:1];
		}
		else if ([typeString isEqualToString:@"sdk"])
		{
			type = KrollUtils::SDK;;
			name = [parts objectAtIndex:0];
			version = [parts objectAtIndex:1];
		}
		else if ([typeString isEqualToString:@"mobilesdk"])
		{
			type = KrollUtils::MOBILESDK;;
			name = [parts objectAtIndex:0];
			version = [parts objectAtIndex:1];
		}
		else
		{
			// Unknown file!
			NSLog(@"Unknown file type for url: %@",url);
			return;
		}
	}
	else
	{
		NSArray* parts = [[[url query] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding] componentsSeparatedByString:@"&"];
		NSEnumerator * partsEnumerator = [parts objectEnumerator];
		NSString *thisPart;
		while ((thisPart = [partsEnumerator nextObject]))
		{
			if ([thisPart isEqualToString:RUNTIME_UUID_FRAGMENT])
			{
				type = KrollUtils::RUNTIME;
			}
			else if ([thisPart isEqualToString:MODULE_UUID_FRAGMENT])
			{
				type = KrollUtils::MODULE;
			}
			else if ([thisPart isEqualToString:SDK_UUID_FRAGMENT])
			{
				type = KrollUtils::SDK;
			}
			else if ([thisPart isEqualToString:MOBILESDK_UUID_FRAGMENT])
			{
				type = KrollUtils::MOBILESDK;
			}
			else if ([thisPart hasPrefix:@"name="])
			{
				name = [thisPart substringFromIndex:5];
			}
			else if ([thisPart hasPrefix:@"version="])
			{
				version = [thisPart substringFromIndex:8];
			}
		}
	}

	NSString* destDir;
	if (type == KrollUtils::MODULE) 
	{
		destDir = [NSString stringWithFormat:@"%@/modules/osx/%@/%@", installDirectory, name, version];
	}
	else if (type == KrollUtils::RUNTIME) 
	{
		destDir = [NSString stringWithFormat:@"%@/runtime/osx/%@", installDirectory, version];
	}
	else if (type == KrollUtils::SDK || type == KrollUtils::MOBILESDK)
	{
		destDir = installDirectory;
	}
	else if (type == KrollUtils::APP_UPDATE)
	{
		destDir = [NSString stringWithUTF8String:app->path.c_str()];
	}
	else
	{
		// Unknown file!
		NSLog(@"Unknown file type for url: %@",url);
		return;
	}

#ifdef DEBUG
	NSLog(@"name=%@,version=%@,module=%d", name, version, type);
	NSLog(@"Installing %@ into %@", path, destDir);
#endif

	[self createDirectory:destDir];
	std::string cmdline = "/usr/bin/ditto --noqtn -x -k --rsrc ";
	cmdline+="\"";
	cmdline+=[path UTF8String];
	cmdline+="\" \"";
	cmdline+=[destDir UTF8String];
	cmdline+="\"";

#ifdef DEBUG
	NSLog(@"Executing: %s", cmdline.c_str());
#endif

	int ec = system(cmdline.c_str());

	if (ec != 0)
	{
		NSLog(@"Could not unzip %@ to %@, because ditto returned a non-zero exitcode (%d)", path, destDir, ec);
		[self bailWithMessage:@"Download file extraction failed. Possibly it was corrupted or couldn't be properly downloaded."];
	}
}

-(void)downloadAndInstall:(Controller*)controller 
{
	printf("downloading jobs: %i\n", [jobs count]);
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	// Download only those jobs which actually need to be downloaded
	int numJobs = (int) [jobs count];
	int current = 0;
	for (int i = 0; i < numJobs; i++)
	{
		Job* job = [jobs objectAtIndex:i];
		if ([job needsDownload])
		{
			current++;
			[self downloadJob:job atIndex:current];
		}
	}

	[progressBar setIndeterminate:NO];
	[progressBar setMinValue:0.0];
	[progressBar setMaxValue:[jobs count] * 2];
	[progressBar setDoubleValue:0.0];

	const char* ext = numJobs > 1 ? "s" : "";
	[controller updateMessage:[NSString stringWithFormat:@"Installing %d file%s", numJobs, ext]];
	for (int i = 0; i < numJobs; i++)
	{
		Job* job = [jobs objectAtIndex:i];
		[progressBar setDoubleValue:((i+1) * 2) - 1];
		[controller updateMessage:[NSString stringWithFormat:@"Installing %d of %d file%s", i + 1, numJobs, ext]];
		[controller install:job];
	}

	[progressBar setDoubleValue:numJobs * 2];
	[controller finishInstallation];
	[controller updateMessage:@"Installation complete"];
	NSLog(@"Installation is complete, exiting after installing %d files", numJobs);
	[pool release];
	[NSApp terminate:self];
}

-(void)finishInstallation
{
	// Write the .installed file
	std::string filePath(FileUtils::Join(app->GetDataPath().c_str(), ".installed", 0));
	[[NSFileManager defaultManager]
		createFileAtPath:[NSString stringWithUTF8String:filePath.c_str()]
		contents:[NSData data]
		attributes:nil];
}

-(void)downloadJob:(Job*)job atIndex:(int)index
{
	[self updateMessage:[NSString stringWithFormat:@"Downloading %d of %d", index, [job totalDownloads]]];
	Downloader *downloader = [[Downloader alloc] initWithURL:[job url] progress:progressBar];
	while ([downloader isDownloadComplete] == NO)
	{
		// this could be more elegant, but it works
		[NSThread sleepUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.2]];
	}

	NSString* filename = [downloader suggestedFilename];
	NSData* data = [downloader data];
	NSString* errorMsg = @"Some files failed to download properly. Cannot continue.";
	if ([filename length] <= 5)
	{
		NSLog(@"Error in handling url \"%@\":", [job url]);
		NSLog(@"File name is too small to specify a file. \"%@\" was made instead.", filename);
		[downloader release];
		[self performSelectorOnMainThread:@selector(bailWithMessage:) withObject:errorMsg waitUntilDone:YES];
	}
	else if ([data length] <= 100)
	{
		NSLog(@"Error in handling url \"%@\":", [job url]);
		NSString * dataString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
		NSLog(@"Response data was too small to be a file. Received \"%@\" instead.", dataString);
		[dataString release];
		[downloader release];
		[self performSelectorOnMainThread:@selector(bailWithMessage:) withObject:errorMsg waitUntilDone:YES];
	}
	else
	{
		NSString *path = [[self temporaryDirectory] stringByAppendingPathComponent:filename];
		[data writeToFile:path atomically:YES];
		[job setPath:path];
		[downloader release];
	}
}

-(void)createInstallerMenu: (NSString*)applicationName
{
	NSString* menuAppName = [NSString stringWithFormat:@"%@ Installer", applicationName];
	NSMenu* mainMenu = [[NSMenu alloc] initWithTitle:@"MainMenu"];
	NSMenuItem* menuItem;
	NSMenu* submenu;

	// The titles of the menu items are for identification purposes only and shouldn't be localized.
	// The strings in the menu bar come from the submenu titles,
	// except for the application menu, whose title is ignored at runtime.
	menuItem = [mainMenu addItemWithTitle:@"Apple" action:NULL keyEquivalent:@""];
	submenu = [[NSMenu alloc] initWithTitle:@"Apple"];
	[NSApp performSelector:@selector(setAppleMenu:) withObject:submenu];
	[mainMenu setSubmenu:submenu forItem:menuItem];


	NSMenu* aMenu = submenu;

	menuItem = [aMenu 
		addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"About", nil), menuAppName]
		action:@selector(orderFrontStandardAboutPanel:)
		keyEquivalent:@""];
	[menuItem setTarget:NSApp];

	[aMenu addItem:[NSMenuItem separatorItem]];

	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Services", nil)
		action:NULL
		keyEquivalent:@""];
	NSMenu* servicesMenu = [[NSMenu alloc] initWithTitle:@"Services"];
	[aMenu setSubmenu:servicesMenu forItem:menuItem];
	[NSApp setServicesMenu:servicesMenu];
	
	[aMenu addItem:[NSMenuItem separatorItem]];

	menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"Hide", nil), menuAppName]
		action:@selector(hide:)
		keyEquivalent:@"h"];
	[menuItem setTarget:NSApp];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Hide Others", nil)
		action:@selector(hideOtherApplications:)
		keyEquivalent:@"h"];
	[menuItem setKeyEquivalentModifierMask:NSCommandKeyMask | NSAlternateKeyMask];
	[menuItem setTarget:NSApp];
	
	menuItem = [aMenu addItemWithTitle:NSLocalizedString(@"Show All", nil)
		action:@selector(unhideAllApplications:)
		keyEquivalent:@""];
	[menuItem setTarget:NSApp];
	
	[aMenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"Quit", nil), menuAppName]
		action:@selector(terminate:)
		keyEquivalent:@"q"];
	[menuItem setTarget:NSApp];
	
	// edit
	menuItem = [mainMenu addItemWithTitle:@"Edit" action:NULL keyEquivalent:@""];
	submenu = [[NSMenu alloc] initWithTitle:NSLocalizedString(@"Edit", @"The Edit menu")];
	[mainMenu setSubmenu:submenu forItem:menuItem];
	menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Undo", nil)
		action:@selector(undo:)
		keyEquivalent:@"z"];
	
	menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Redo", nil)
		action:@selector(redo:)
		keyEquivalent:@"Z"];
	
	[submenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Cut", nil)
		action:@selector(cut:)
		keyEquivalent:@"x"];
	
	menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Copy", nil)
		action:@selector(copy:)
		keyEquivalent:@"c"];
	
	menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Paste", nil)
		action:@selector(paste:)
		keyEquivalent:@"v"];
	
	menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Delete", nil)
		action:@selector(delete:)
		keyEquivalent:@""];

	menuItem = [submenu addItemWithTitle: NSLocalizedString(@"Select All", nil)
		action: @selector(selectAll:)
		keyEquivalent: @"a"];

	// window
	menuItem = [mainMenu addItemWithTitle:@"Window" action:NULL keyEquivalent:@""];
	submenu = [[NSMenu alloc] initWithTitle:NSLocalizedString(@"Window", @"The Window menu")];
	[mainMenu setSubmenu:submenu forItem:menuItem];
	menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Minimize", nil)
		action:@selector(performMinimize:)
		keyEquivalent:@"m"];
	
	menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Zoom", nil)
		action:@selector(performZoom:)
		keyEquivalent:@""];
	
	[submenu addItem:[NSMenuItem separatorItem]];
	
	menuItem = [submenu addItemWithTitle:NSLocalizedString(@"Bring All to Front", nil)
		action:@selector(arrangeInFront:)
		keyEquivalent:@""];
	
	[NSApp setWindowsMenu:submenu];
	[NSApp setMainMenu:mainMenu];
	[mainMenu release];
}

-(void)awakeFromNib
{ 
	[NSApp setDelegate:self];
}

-(void)setupDialogs
{
	NSString* appName = @"Unknown";
	if (!app->name.empty())
		appName = [NSString stringWithUTF8String:app->name.c_str()];

	NSString* appVersion = @"Unknown";
	if (!app->version.empty() && updateFile == nil)
	{
		appVersion = [NSString stringWithUTF8String:app->version.c_str()];
	}
	else if (!app->version.empty())
	{
		appVersion = [NSString stringWithUTF8String:app->version.c_str()];
		appVersion = [appVersion stringByAppendingString:@" (Update)"];
	}

	NSString* appPublisher = @"Unknown";
	if (!app->publisher.empty())
		appPublisher = [NSString stringWithUTF8String:app->publisher.c_str()];

	NSString* appURL = @"Unknown";
	if (!app->url.empty())
		appURL = [NSString stringWithUTF8String:app->url.c_str()];

	[self createInstallerMenu:appName];
	[progressAppName setStringValue:appName];
	[introAppName setStringValue:appName];
	[progressAppVersion setStringValue:appVersion];
	[introAppVersion setStringValue:appVersion];
	[progressAppPublisher setStringValue:appPublisher];
	[introAppPublisher setStringValue:appPublisher];
	[progressAppURL setStringValue:appURL];
	[introAppURL setStringValue:appURL];

	[introAppVersion setFont:[NSFont boldSystemFontOfSize:12]];
	[introAppPublisher setFont:[NSFont boldSystemFontOfSize:12]];
	[introAppURL setFont:[NSFont boldSystemFontOfSize:12]];

	if (!app->image.empty())
	{
		NSImage* img = [[NSImage alloc] initWithContentsOfFile:
			[NSString stringWithUTF8String:app->image.c_str()]];
		if ([img isValid])
		{
			[progressImage setImage:img];
			[introImage setImage:img];
		}
	}
}

-(void)showIntroDialog
{
	string licensePathString(FileUtils::Join(app->path.c_str(), LICENSE_FILENAME, 0));
	NSString* licensePath = [NSString stringWithUTF8String:licensePathString.c_str()];
	NSFileManager* fm = [NSFileManager defaultManager];
	if ([fm fileExistsAtPath:licensePath])
	{
		NSString* licenseText = [NSString
			stringWithContentsOfFile:licensePath
			encoding:NSUTF8StringEncoding
			error:nil];
		NSAttributedString* licenseAttrText = [[NSAttributedString alloc] initWithString:licenseText];
		[[introLicenseText textStorage] setAttributedString:licenseAttrText];
		[introLicenseText setEditable:NO];
	}
	else
	{
		[introLicenseLabel setHidden:YES];
		[introLicenseBox setHidden:YES];
		NSRect frame = [introWindow frame];
		frame.size.width = 650;
		frame.size.height = 275;
		[introWindow setMinSize:frame.size];
		[introWindow setMaxSize:frame.size];
		[introWindow setShowsResizeIndicator:NO];
		[introWindow setFrame:frame display:YES];
	}

	// Hide the progress Window.
	[progressWindow orderOut:self];
	[introWindow center];
	[introWindow makeKeyAndOrderFront:self];
}

- (BOOL)canBecomeKeyWindow
{
	return YES;
}

- (BOOL)canBecomeMainWindow
{
	return YES;
}

-(void)applicationDidFinishLaunching:(NSNotification *) notif
{
	jobs = [[NSMutableArray alloc] init];
	skipIntroDialog = NO;;
	updateFile = nil;
	NSString *appPath = nil;

	NSArray* args = [[NSProcessInfo processInfo] arguments];
	int count = [args count];
	for (int i = 1; i < count; i++)
	{
		NSString* arg = [args objectAtIndex:i];
		if ([arg isEqual:@"-appPath"] && count > i+1)
		{
			appPath = [args objectAtIndex:i+1];
			i++;
		}
		else if ([arg isEqual:@"-updateFile"] && count > i+1)
		{
			updateFile = [args objectAtIndex:i+1];
			[updateFile retain];
			i++;
		}
		else if ([arg isEqual:@"-quiet"])
		{
			skipIntroDialog = YES;
		}
		else
		{
			[jobs addObject:[[Job alloc] init:arg]];
		}
	}

	if (!appPath)
		[self bailWithMessage:@"Sorry, but the installer was not given the application path."];

	if (!updateFile)
	{
		app = Application::NewApplication([appPath UTF8String]);
	}
	else
	{
		if (!FileUtils::IsFile([updateFile UTF8String]))
			[self bailWithMessage:@"Could not find specified update file."];

		skipIntroDialog = YES;
		app = Application::NewApplication([updateFile UTF8String], [appPath UTF8String]);
		[jobs addObject:[[Job alloc]
			initUpdate:[NSString stringWithUTF8String:app->GetUpdateURL().c_str()]]];

		// Remove the update file as soon as possible, so that if the installation
		// fails the application will still start. We can always fetch the update
		// later.
		[[NSFileManager defaultManager] removeFileAtPath:updateFile handler:nil];
	}

	std::string tempDir = FileUtils::GetTempDirectory();
	temporaryDirectory = [NSString stringWithUTF8String:tempDir.c_str()];
	[self createDirectory: temporaryDirectory];
	[temporaryDirectory retain];

	// Check to see if we can write to the system install location -- if so install there
	std::string systemRuntimeHome = FileUtils::GetSystemRuntimeHomeDirectory();
	std::string userRuntimeHome = FileUtils::GetUserRuntimeHomeDirectory();

	installDirectory = [NSString stringWithUTF8String:systemRuntimeHome.c_str()];
	if ((!FileUtils::IsDirectory(systemRuntimeHome) && 
			[[NSFileManager defaultManager] isWritableFileAtPath:[installDirectory stringByDeletingLastPathComponent]]) ||
		[[NSFileManager defaultManager] isWritableFileAtPath:installDirectory])
	{
		installDirectory = [NSString stringWithUTF8String:systemRuntimeHome.c_str()];
	}
	else
	{
		// Cannot write to system-wide install location -- install to user directory
		installDirectory = [NSString stringWithUTF8String:userRuntimeHome.c_str()];
	}
	[installDirectory retain];

	[self setupDialogs];
	if (skipIntroDialog)
	{
		[self continueIntro:self];
		return;
	}
	else
	{
		[self showIntroDialog];
	}
}

-(IBAction)cancelProgress: (id)sender
{
	[progressCancelButton setEnabled:NO];
	[progressBar setDoubleValue:100.0];
	[progressText setStringValue:@"Cancelling..."];
	[NSApp terminate:self];
}

-(IBAction)cancelIntro:(id)sender
{
	[NSApp terminate:self];
}

-(IBAction)continueIntro:(id)sender;
{
	if (!skipIntroDialog)
		[introWindow orderOut:self];

	// Always show the progress view, even if we skip the intro dialog.
	// Otherwise the user won't know what is going on or be able to cancel
	// the download.
	[progressText setStringValue:@"Connecting to download site..."];
	[progressBar setUsesThreadedAnimation:NO];
	[progressBar setIndeterminate:NO];
	[progressBar setMinValue:0.0];
	[progressBar setMaxValue:100.0];
	[progressBar setDoubleValue:0.0];

	[progressWindow center];
	if ([jobs count] > 0)
	{
		[progressWindow makeKeyAndOrderFront:progressWindow];
		[NSApp activateIgnoringOtherApps:YES];
	}

	[NSThread detachNewThreadSelector:@selector(downloadAndInstall:) 
		toTarget:self
		withObject:self];
}

@end
