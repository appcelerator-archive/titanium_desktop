/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#import "Downloader.h"

@implementation Downloader

-(id)initWithURL:(NSURL*)url progress:(NSProgressIndicator*)p
{
	self = [super init];
	if (self)
	{
		progress = [p retain];
		[progress startAnimation:self];
		[self performSelectorOnMainThread:@selector(startUrlRequest:) withObject:url waitUntilDone:YES];
	}
	return self;
}

#define VAL(str) #str
#define STRING(str) VAL(str)

- (void)startUrlRequest: (NSURL *) url;
{
	NSLog(@"Starting URL request for %@",url);
	[data release];
	[downloadRequest release];
	downloadRequest = [[NSMutableURLRequest alloc] initWithURL:url];
	[downloadRequest setTimeoutInterval:10.0];
	userAgent = [NSString stringWithFormat:@"Mozilla/5.0 (compatible; Titanium_Downloader/%s; Mac)",STRING(_PRODUCT_VERSION)];
	[downloadRequest setValue:userAgent forHTTPHeaderField:@"User-Agent"];
	data = [[NSMutableData alloc] init];
	downloadConnection = [[NSURLConnection alloc] initWithRequest:downloadRequest delegate:self];
	//NOTE: do not call start!! it's automatically called and you will segfault if you call it
	//NOTE: do not release downloadRequest!
	[self setCompleted:NO];
}

-(void)dealloc
{
	[downloadRequest release];
	[suggestedFileName release];
	[downloadConnection release];
	[progress release];
	[data release];
	[super dealloc];
}

-(NSData*)data
{
	return data;
}

- (NSString *)suggestedFileName 
{
	return suggestedFileName;
}

- (BOOL)completed;
{
	return completed;
}

- (void)setCompleted:(BOOL)value;
{
	completed = value;
	if (completed) 
	{
		[progress stopAnimation:self];
	} 
	else 
	{
		[progress startAnimation:self];
	}
}

-(BOOL)isDownloadComplete
{
	return completed;
}

#pragma mark NSURLConnection delegate methods

//
// NSURLConnection Delegate Methods
//

// Part of this code courtesy of google mac toolkit

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
	[data setLength:0];
	expectedBytes = [response expectedContentLength];
	[suggestedFileName release];
	suggestedFileName = [[[[response URL] path] lastPathComponent] retain];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)newData
{
	[data appendData:newData];
	if (expectedBytes != 0) 
	{
		[progress setIndeterminate:NO];
		[progress setMaxValue:(double)expectedBytes];
		[progress setDoubleValue:(double)[data length]];
	}
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	[progress setIndeterminate:YES];
	[self setCompleted:YES];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection;
{
	[self setCompleted:YES];
}

@end
