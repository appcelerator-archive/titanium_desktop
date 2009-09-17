/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#import <Cocoa/Cocoa.h>
#import "Downloader.h"
#import <utils.h>
using namespace KrollUtils;
using KrollUtils::Application;
using KrollUtils::SharedApplication;
using KrollUtils::KComponentType;
SharedApplication app;

@interface Job : NSObject {
	NSURL* url; 
	NSString* path; 
	BOOL isUpdate; 
}
-(Job*)init:(NSString*)pathOrURL;
-(Job*)initUpdate:(NSString*)pathOrURL;
-(NSURL*)url;
-(NSString*)path;
-(void)setPath:(NSString*)newPath;
-(BOOL)needsDownload;
-(BOOL)isUpdate;
-(int)totalDownloads;
-(int)totalJobs;
@end

@interface Controller : NSObject {
	IBOutlet NSWindow* progressWindow;
	IBOutlet NSProgressIndicator* progressBar;
	IBOutlet NSButton* progressCancelButton;
	IBOutlet NSTextField* progressText;
	IBOutlet NSImageView* progressImage;
	IBOutlet NSTextField* progressAppName;
	IBOutlet NSTextField* progressAppVersion;
	IBOutlet NSTextField* progressAppPublisher;
	IBOutlet NSTextField* progressAppURL;

	IBOutlet NSWindow* introWindow;
	IBOutlet NSTextField* introLicenseLabel;
	IBOutlet NSBox* introLicenseBox;
	IBOutlet NSTextView* introLicenseText;

	IBOutlet NSImageView* introImage;
	IBOutlet NSTextField* introAppName;
	IBOutlet NSTextField* introAppVersion;
	IBOutlet NSTextField* introAppPublisher;
	IBOutlet NSTextField* introAppURL;

	NSMutableArray *jobs;
	NSString *temporaryDirectory;
	NSString *installDirectory;
	NSString *updateFile;
	BOOL quiet;
}

-(IBAction)cancelProgress:(id)sender;
-(IBAction)cancelIntro:(id)sender;
-(IBAction)continueIntro:(id)sender;
-(void)updateMessage:(NSString*)msg;
-(NSString*)temporaryDirectory;
-(NSString*)installDirectory;
-(void)finishInstallation;
-(void)downloadAndInstall:(Controller*)controller;
-(void)install:(Job*)job;
-(void)downloadJob:(Job*)job atIndex:(int)index;
-(void)createInstallerMenu:(NSString*)applicationName;
-(void)showIntroDialog: (NSString*)appName
	path:(NSString*)appPath
	version:(NSString*)appVersion
	publisher:(NSString*)appPublisher
	url:(NSString*)appURL
	image:(NSString*)appImage;

@end
