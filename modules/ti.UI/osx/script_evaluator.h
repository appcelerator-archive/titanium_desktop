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

@interface ScriptEvaluator : NSObject
{
	Host* host;
}
-(ScriptEvaluator*) initWithHost:(Host*)hostin;
-(NSString*) getModuleName:(NSString*)typeString;
-(BOOL) matchesMimeType:(NSString*)mimeType;
-(void) evaluate:(NSString *)mimeType sourceCode:(NSString*)sourceCode context:(void *)context;
@end
