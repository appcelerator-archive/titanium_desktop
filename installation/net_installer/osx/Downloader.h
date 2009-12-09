/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#import <Cocoa/Cocoa.h>


@interface Downloader :  NSObject {
	NSString* suggestedFilename;
	NSString* userAgent;
	NSMutableURLRequest* downloadRequest;
	NSURLConnection* downloadConnection;
	NSProgressIndicator* progress;
	NSMutableData* data;

	long long expectedBytes;
	BOOL completed;
}
-(id)initWithURL:(NSURL*)url progress:(NSProgressIndicator*)p;
-(BOOL)isDownloadComplete;
-(BOOL)completed;
-(void)setCompleted:(BOOL)value;
-(NSString*)suggestedFilename;
-(NSData*)data;
@end
