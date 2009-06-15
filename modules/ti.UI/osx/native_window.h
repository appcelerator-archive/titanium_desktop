/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _NATIVE_WINDOW_H_
#define _NATIVE_WINDOW_H_

#import "../ui_module.h"
#import <Cocoa/Cocoa.h>
#import "ti_app.h"
#import "WebInspector.h" 

@class WebViewDelegate;

using namespace ti;

@interface NativeWindow : NSWindow {
	BOOL canReceiveFocus;
	WindowConfig* config;
	WebView* webView;
	WebViewDelegate* delegate;
	BOOL requiresDisplay;
	UserWindow* userWindow;
	WebInspector *inspector;
	BOOL closed;
	BOOL fullscreen;
	BOOL focused;
	NSRect savedFrame;
}
- (void)setupDecorations:(WindowConfig*)config host:(Host*)h userwindow:(UserWindow*)uw;
- (void)setTransparency:(double)transparency;
- (void)setFullScreen:(BOOL)yn;
- (void)close;
- (void)open;
- (void)frameLoaded;
- (WebView*)webView;
- (WindowConfig*)config;
- (UserWindow*)userWindow;
- (void)setInitialWindow:(BOOL)yn;
- (void)showInspector;
- (void)fireWindowEvent:(UserWindowEvent)event;
@end

#endif
