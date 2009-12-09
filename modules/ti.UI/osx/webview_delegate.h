/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import "../ui_module.h"
#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import <WebKit/WebViewPrivate.h>
#import <WebKit/WebInspector.h>
#import <WebKit/WebScriptDebugDelegate.h>
#import <WebKit/WebScriptObject.h>
#import <WebKit/WebPreferencesPrivate.h>

@class NativeWindow;
@interface WebViewDelegate : NSObject {
	NativeWindow *window;
	Host *host;
	WebInspector *inspector;
	BOOL initialDisplay;
	std::map<WebFrame*, KObjectRef> *frameToGlobalObject;
	Logger *logger;
}
-(id)initWithWindow:(NativeWindow*)window;
-(void)setupPreferences;
-(void)registerGlobalObject:(KObjectRef) globalObject forFrame:(WebFrame *)frame;
-(KObjectRef)registerJSContext:(JSGlobalContextRef)context forFrame:(WebFrame*)frame;
-(BOOL)isGlobalObjectRegisteredForFrame:(WebFrame*) frame;
-(KObjectRef)globalObjectForFrame:(WebFrame*) frame;
-(void)deregisterGlobalObjectForFrame:(WebFrame *)frame;
@end
