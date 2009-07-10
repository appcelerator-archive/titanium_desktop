/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import <WebKit/WebKit.h>
#include "../ui_module.h"

@implementation TitaniumProtocols

+ (BOOL)canInitWithRequest:(NSURLRequest *)theRequest 
{
	NSString *theScheme = [[theRequest URL] scheme];
	return [theScheme isEqual:@"app"] || [theScheme isEqual:@"ti"];
}

+(NSURLRequest *)canonicalRequestForRequest:(NSURLRequest *)request 
{
    return request;
}

+(NSString *)mimeTypeFromExtension:(NSString *)ext
{
	NSString *mime = @"application/octet-stream";
	
	if ([ext isEqualToString:@"png"])
	{
		mime = @"image/png";
	}
	else if ([ext isEqualToString:@"gif"])
	{
		mime = @"image/gif"; 
	}
	else if ([ext isEqualToString:@"jpg"])
	{
		mime = @"image/jpeg";
	}
	else if ([ext isEqualToString:@"jpeg"])
	{
		mime = @"image/jpeg";
	}
	else if ([ext isEqualToString:@"ico"])
	{
		mime = @"image/x-icon";
	}
	else if ([ext isEqualToString:@"html"])
	{
		mime = @"text/html";
	}
	else if ([ext isEqualToString:@"htm"])
	{
		mime = @"text/html";
	}
	else if ([ext isEqualToString:@"text"])
	{
		mime = @"text/plain";
	}
	else if ([ext isEqualToString:@"js"])
	{
		mime = @"text/javascript";
	}
	else if ([ext isEqualToString:@"json"])
	{
		mime = @"application/json";
	}
	else if ([ext isEqualToString:@"css"])
	{
		mime = @"text/css";
	}
	else if ([ext isEqualToString:@"xml"])
	{
		mime = @"text/xml";
	}
	return mime;
}

-(void)startLoading
{
	static Logger* logger = Logger::Get("UI.TitaniumProtocols");

	id<NSURLProtocolClient> client = [self client];
	NSURL* url = [[self request] URL];
	std:string urlString = [[url absoluteString] UTF8String];
	std::string path = ti::URLToPathOrURL(urlString);
	NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];

	NSError* error;
	NSData *data = [NSData dataWithContentsOfFile:nsPath options:0 error:&error];
	if (data == nil) { // File doesn't exist

		logger->Error("Error finding %s", [nsPath UTF8String]);
		[client URLProtocol:self didFailWithError:[NSError
			errorWithDomain:NSURLErrorDomain
			code:NSURLErrorResourceUnavailable
			userInfo:nil]];
		[client URLProtocolDidFinishLoading:self];
		return;

	} else { // It loaded!
		NSURLResponse *response = [[NSURLResponse alloc]
			initWithURL:url
			MIMEType:[TitaniumProtocols mimeTypeFromExtension:[nsPath pathExtension]]
			expectedContentLength:[data length]
			textEncodingName:@"utf-8"];

		[client URLProtocol:self
			didReceiveResponse:response
			cacheStoragePolicy:NSURLCacheStorageAllowed];
		[client URLProtocol:self didLoadData:data];

		[client URLProtocolDidFinishLoading:self];
		[response release];
	}
}

-(void)stopLoading 
{
}

@end
