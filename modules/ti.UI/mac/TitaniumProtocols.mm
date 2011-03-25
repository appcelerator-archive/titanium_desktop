/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import "TitaniumProtocols.h"

#import <CoreFoundation/CFString.h>
#import <WebKit/WebKit.h>

#include <kroll/kroll.h>

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

    NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
    data = [NSData dataWithContentsOfFile:nsPath options:0 error:&error];
    mimeType = [TitaniumProtocols mimeTypeFromExtension:
        [nsPath pathExtension]];
    cachePolicy = NSURLCacheStorageAllowed;

    if (data == nil)
        logger->Error("Error finding %s", [nsPath UTF8String]);

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
