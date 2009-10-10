/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#import <Cocoa/Cocoa.h>


@interface Downloader :  NSObject {
	NSString* suggestedFileName;
	NSString* userAgent;
	NSMutableURLRequest* downloadRequest;
	NSURLConnection* downloadConnection;
	NSProgressIndicator* progress;
	int bytesRetrievedSoFar;
	long long expectedBytes;
	BOOL completed;
	NSMutableData* data;
}
-(id)initWithURL:(NSURL*)url progress:(NSProgressIndicator*)p;
-(BOOL)isDownloadComplete;
-(BOOL)completed;
-(void)setCompleted:(BOOL)value;
-(NSString *)suggestedFileName;

-(NSData*)data;
@end
