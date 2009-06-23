/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "osx_desktop.h"

@interface ScreenshotTaker : NSObject {
	
};

- (void)takeScreenshot:(NSString*)path;

@end

@implementation ScreenshotTaker;

- (NSImage*) imageFromCGImageRef:(CGImageRef)image
{
    NSRect imageRect = NSMakeRect(0.0, 0.0, 0.0, 0.0);
    CGContextRef imageContext = nil;
    NSImage* newImage = nil;
	
    // Get the image dimensions.
    imageRect.size.height = CGImageGetHeight(image);
    imageRect.size.width = CGImageGetWidth(image);
	
    // Create a new image to receive the Quartz image data.
    newImage = [[[NSImage alloc] initWithSize:imageRect.size] autorelease];
    [newImage lockFocus];
	
    // Get the Quartz context and draw.
    imageContext = (CGContextRef)[[NSGraphicsContext currentContext]
								  graphicsPort];
    CGContextDrawImage(imageContext, *(CGRect*)&imageRect, image);
    [newImage unlockFocus];
	
    return newImage;
}

void exportCGImage2PNGFileWithDestination(CGImageRef image, CFURLRef url){
    float resolution = 144.0;
    CFTypeRef keys[2];
    CFTypeRef values[2];
    CFDictionaryRef options = NULL;
    CGImageDestinationRef imageDestination = CGImageDestinationCreateWithURL(url,kUTTypePNG,1,NULL);
    if(imageDestination == NULL){
        return;
    }
    keys[0] = kCGImagePropertyDPIWidth;
    keys[1] = kCGImagePropertyDPIHeight;
	
    values[0] = CFNumberCreate(NULL,kCFNumberFloatType,&resolution);
    values[1] = values[0];
	
    options = CFDictionaryCreate(NULL,
                                 keys,
                                 values,
                                 2,
                                 &kCFTypeDictionaryKeyCallBacks,
                                 &kCFTypeDictionaryValueCallBacks);
    CFRelease(values[0]);
	
    CGImageDestinationAddImage(imageDestination,image,options);
    CFRelease(options);
	
    CGImageDestinationFinalize(imageDestination);
    CFRelease(imageDestination);
}

- (void)takeScreenshot:(NSString*)path
{
	
	CGImageRef screenShot = CGWindowListCreateImage(CGRectInfinite, kCGWindowListOptionOnScreenOnly, kCGNullWindowID, kCGWindowImageDefault);
	NSURL* url = [NSURL fileURLWithPath:path];
	exportCGImage2PNGFileWithDestination(screenShot, (CFURLRef)url);
}


@end

namespace ti
{
	OSXDesktop::OSXDesktop()
	{
	}

	OSXDesktop::~OSXDesktop()
	{
	}

	bool OSXDesktop::OpenApplication(std::string& app)
	{
		NSDictionary *dictionary = [[NSProcessInfo processInfo] environment];
		NSMutableDictionary *remember = [[NSMutableDictionary alloc] init];
		// unset any KR_ environment variables and the library path
		// so it doesn't interfere with process we're launching 
		// (reset below)
		NSEnumerator * ourDictionaryKeyEnumerator = [dictionary keyEnumerator];
		while (id key = [ourDictionaryKeyEnumerator nextObject])
		{
			if ([key hasPrefix:@"KR_"])
			{
				[remember setObject:[dictionary objectForKey:key] forKey:key];
			}
		}
		[remember setObject:[dictionary objectForKey:@"DYLD_LIBRARY_PATH"] forKey:@"DYLD_LIBRARY_PATH"];
		
		NSWorkspace* ws = [NSWorkspace sharedWorkspace];
		NSString *name = [NSString stringWithCString:app.c_str() encoding:NSUTF8StringEncoding];
#ifdef DEBUG
		NSLog(@"launching external app: %@",name);
#endif
		BOOL result = [ws launchApplication:name];

		NSEnumerator * ourRememberKeyEnumerator = [remember keyEnumerator];
		while (id key = [ourRememberKeyEnumerator nextObject])
		{
			id value = [remember objectForKey:key];
			setenv([key UTF8String],[value UTF8String],1);
		}
#ifdef DEBUG
		NSLog(@"launched external app, returned = %d",result);
#endif
		[remember release];
		return result;
	}

	bool OSXDesktop::OpenURL(std::string& url)
	{
		NSWorkspace * ws = [NSWorkspace sharedWorkspace];
		BOOL wasOpened = [ws openURL:[NSURL URLWithString:[NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding]]];
		return wasOpened;
	}
	
	void OSXDesktop::TakeScreenshot(std::string &screenshotFile)
	{
		ScreenshotTaker *taker = [[ScreenshotTaker alloc] init];
		[taker takeScreenshot:[NSString stringWithCString:screenshotFile.c_str() encoding:NSUTF8StringEncoding]];
		[taker release];
	}
}
