/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import <WebKit/WebKit.h>
#include "../ui_module.h"

static NSString* GetRegisteredMimeTypeFromExtension(NSString* ext)
{
	CFRef<CFStringRef> uti(UTTypeCreatePreferredIdentifierForTag(
		kUTTagClassFilenameExtension, (CFStringRef)ext, 0));
	if (!uti.get())
		return nil;

	CFStringRef registeredType = UTTypeCopyPreferredTagWithClass(
		uti.get(), kUTTagClassMIMEType);
	if (!registeredType)
		return nil;

	NSString* mimeType = NSMakeCollectable(registeredType);
	return [mimeType autorelease];
}

@implementation TitaniumProtocols

+(BOOL)canInitWithRequest:(NSURLRequest*)theRequest 
{
	NSString* theScheme = [[theRequest URL] scheme];
	return [theScheme isEqual:@"app"] || [theScheme isEqual:@"ti"];
}

+(NSURLRequest*)getNormalizedRequest:(NSURLRequest*)request
{
	std::string url = [[[request URL] absoluteString] UTF8String];
	std::string normalized = URLUtils::NormalizeURL(url);
	if (url != normalized)
	{
		NSURL* newNSURL = [NSURL URLWithString:
			[NSString stringWithUTF8String:normalized.c_str()]];
		NSURLRequest* newRequest = [NSURLRequest requestWithURL:newNSURL];
		return newRequest;
	}
	else
	{
		return request;
	}
}

+(NSURLRequest*)canonicalRequestForRequest:(NSURLRequest*)request 
{
	return [TitaniumProtocols getNormalizedRequest:request];
}

+(NSString*)mimeTypeFromExtension:(NSString*)ext
{
	NSString* mimeType = GetRegisteredMimeTypeFromExtension(ext);
	if (mimeType)
		return mimeType;

	if ([ext isEqualToString:@"png"])
		return @"image/png";
	else if ([ext isEqualToString:@"gif"])
		return @"image/gif"; 
	else if ([ext isEqualToString:@"jpg"])
		return @"image/jpeg";
	else if ([ext isEqualToString:@"jpeg"])
		return @"image/jpeg";
	else if ([ext isEqualToString:@"ico"])
		return @"image/x-icon";
	else if ([ext isEqualToString:@"html"])
		return @"text/html";
	else if ([ext isEqualToString:@"htm"])
		return @"text/html";
	else if ([ext isEqualToString:@"text"])
		return @"text/plain";
	else if ([ext isEqualToString:@"js"])
		return @"text/javascript";
	else if ([ext isEqualToString:@"json"])
		return @"application/json";
	else if ([ext isEqualToString:@"css"])
		return @"text/css";
	else if ([ext isEqualToString:@"xml"])
		return @"text/xml";
	else if ([ext isEqualToString:@"pdf"])
		return @"application/pdf";
	else if ([ext isEqualToString:@"m4v"] )
		return @"video/x-m4v";
	else if([ext isEqualToString:@"m4p"])
		return @"audio/x-m4p";
	else if([ext isEqualToString:@"swf"])
		return @"application/x-shockwave-flash";
	else
		return @"application/octet-stream";
}

-(NSData*)preprocessRequest:(const char*)url returningMimeType:(NSString**)mimeType
{
	static Logger* logger = Logger::Get("UI.TitaniumProtocols");
	KObjectRef scope = new StaticBoundObject();
	KObjectRef headers = new StaticBoundObject();
	scope->Set("httpHeaders", Value::NewObject(headers));

	NSDictionary *httpHeaders = [[self request] allHTTPHeaderFields];
	for (NSString* header in [httpHeaders allKeys])
	{
		NSString* value = (NSString*) [httpHeaders valueForKey:header];
		headers->SetString([header UTF8String], [value UTF8String]);
	}

	try
	{
		AutoPtr<PreprocessData> result = 
			Script::GetInstance()->Preprocess(url, scope);
		NSData* data = [NSData 
			dataWithBytes:(void *) result->data->Get()
			length:result->data->Length()];
		*mimeType = [NSString stringWithUTF8String:result->mimeType.c_str()];

		return data;
	}
	catch (ValueException& e)
	{
		logger->Error("Error in preprocessing: %s", e.ToString().c_str());
	}
	catch (...)
	{
		logger->Error("Unknown Error in preprocessing");
	}
	return nil;
}

-(void)startLoading
{
	static Logger* logger = Logger::Get("UI.TitaniumProtocols");
	id<NSURLProtocolClient> client = [self client];
	NSURL* url = [[self request] URL];

	// First check if this is the non-canonical version of this request.
	// If it is, we redirect to the canonical version.
	NSURLRequest* normalized = [TitaniumProtocols getNormalizedRequest:[self request]];
	if (normalized != [self request])
	{
		NSURLResponse* response = [[NSURLResponse alloc]
			initWithURL:url
			MIMEType:@"text/plain"
			expectedContentLength:0
			textEncodingName:@"utf-8"];
		[client 
			URLProtocol:self
			wasRedirectedToRequest:normalized
			redirectResponse:response];
		return;
	}
	

	// This is a canonical request, so try to load the file it represents.
	std::string urlString([[url absoluteString] UTF8String]);
	std::string path(URLUtils::URLToPath(urlString));
	NSError* error = nil;
	NSData* data = nil;
	NSString* mimeType = nil;
	NSURLCacheStoragePolicy cachePolicy;

	if (Script::GetInstance()->CanPreprocess(urlString.c_str()))
	{
		data = [self 
			preprocessRequest:urlString.c_str()
			returningMimeType:&mimeType];
		cachePolicy = NSURLCacheStorageNotAllowed;
	}
	else
	{
		NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
		data = [NSData dataWithContentsOfFile:nsPath options:0 error:&error];
		mimeType = [TitaniumProtocols mimeTypeFromExtension:
			[nsPath pathExtension]];
		cachePolicy = NSURLCacheStorageAllowed;

		if (data == nil)
			logger->Error("Error finding %s", [nsPath UTF8String]);
	}

	if (data == nil) // File doesn't exist
	{ 
		[client URLProtocol:self didFailWithError:[NSError
			errorWithDomain:NSURLErrorDomain
			code:NSURLErrorResourceUnavailable
			userInfo:nil]];
		[client URLProtocolDidFinishLoading:self];
		return;
	}
	else // It loaded!
	{ 
		NSURLResponse* response = [[NSURLResponse alloc]
			initWithURL:url
			MIMEType:mimeType
			expectedContentLength:[data length]
			textEncodingName:@"utf-8"];

		[client URLProtocol:self
			didReceiveResponse:response
			cacheStoragePolicy:cachePolicy];
		[client URLProtocol:self didLoadData:data];

		[client URLProtocolDidFinishLoading:self];
		[response release];
	}
}

-(void)stopLoading 
{
}

@end
