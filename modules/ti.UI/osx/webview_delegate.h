/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import "preinclude.h"
#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import "WebViewPrivate.h"
#import "WebInspector.h"
#import "WebScriptDebugDelegate.h"
#import "WebScriptObject.h"
#import "WebPreferencesPrivate.h"

@class NativeWindow;

@interface WebViewDelegate : NSObject {
	NativeWindow *window;
	WebView *webView;
	Host *host;
	NSURL *url;
	WebInspector *inspector;
	BOOL initialDisplay;
	std::map<WebFrame*, SharedKObject> *frames; // this is the frame to clear tracking
}
-(id)initWithWindow:(NativeWindow*)window host:(Host*)h;
-(void)setURL:(NSURL*)url;
-(NSURL*)url;
@end
