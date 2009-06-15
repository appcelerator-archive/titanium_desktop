/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import "ti_protocol.h"
#import "app_protocol.h"
#import "ti_app.h"

@implementation TiProtocol

+ (NSString*) specialProtocolScheme {
	return @"ti";
}

+ (void) registerSpecialProtocol {
	static BOOL inited = NO;
	if ( ! inited ) {
		[NSURLProtocol registerClass:[TiProtocol class]];
		// SECURITY FLAG: this will allow apps to have the same security
		// as local files (like cross-domain XHR requests).  we should 
		// make sure this is part of the upcoming security work
		[WebView registerURLSchemeAsLocal:[self specialProtocolScheme]];
		inited = YES;
	}
}

+ (BOOL)canInitWithRequest:(NSURLRequest *)theRequest 
{
	NSString *theScheme = [[theRequest URL] scheme];
	return [theScheme isEqual:@"ti"];
}

+(NSURLRequest *)canonicalRequestForRequest:(NSURLRequest *)request {
	return request;
}

- (void)load:(NSURL*)url path:(NSString*)fullpath
{
	id<NSURLProtocolClient> client = [self client];
	NSData *data = [[NSData alloc] initWithContentsOfFile:fullpath];
	NSString *ext = [fullpath pathExtension];
	NSString *mime = [AppProtocol mimeTypeFromExtension:ext];
	NSURLResponse *response = [[NSURLResponse alloc] initWithURL:url MIMEType:mime expectedContentLength:-1 textEncodingName:@"utf-8" ];
	[client URLProtocol:self didReceiveResponse:response cacheStoragePolicy:NSURLCacheStorageAllowed];

	if (data != nil && [data length] > 0) {
		[client URLProtocol:self didLoadData:data];
	}

	[client URLProtocolDidFinishLoading:self];
	[response release];
	[data release];
}

- (void)startLoading
{
	NSURLRequest *request = [self request];
	NSURL *url = [request URL];

	NSString *hostpart = [[[url host] stringByReplacingOccurrencesOfString:@"." withString:@""] lowercaseString];
	NSString *pathpart = [url path];
	NSString *normpath = [pathpart stringByReplacingOccurrencesOfString:@".." withString:@""];
	kroll::Host *host = [[TiApplication instance] host];

	std::string basedir;

	if ([hostpart isEqualTo:@"runtime"])
	{
		basedir = host->GetRuntimePath();
	}
	else
	{
		std::string modulename([hostpart UTF8String]);
		SharedPtr<Module> module = host->GetModuleByName(modulename);

		if (!module.isNull())
		{
			basedir = module->GetPath();
		}
	}

	Logger* logger = Logger::Get("UI.TiProtocol");
	
	if (!basedir.empty())
	{
		std::string resourcepath = kroll::FileUtils::Join(basedir.c_str(),[normpath UTF8String],NULL);

		logger->Debug("RESOURCE PATH = %s", resourcepath.c_str());

		if (kroll::FileUtils::IsFile(resourcepath))
		{
			NSString *fullpath = [NSString stringWithCString:resourcepath.c_str()];
			[self load:url path:fullpath];
			return;
		}
	}

	logger->Error("Error finding %s", [[url absoluteString] UTF8String]);

	// File doesn't exist
	int resultCode = NSURLErrorResourceUnavailable;
	id<NSURLProtocolClient> client = [self client];
	[client URLProtocol:self didFailWithError:[NSError errorWithDomain:NSURLErrorDomain code:resultCode userInfo:nil]];
	[client URLProtocolDidFinishLoading:self];
}

- (void)stopLoading {
}

@end
