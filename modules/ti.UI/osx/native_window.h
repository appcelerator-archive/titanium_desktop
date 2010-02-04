/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _NATIVE_WINDOW_H_
#define _NATIVE_WINDOW_H_

#import "../ui_module.h"
#import <Cocoa/Cocoa.h>
#import <WebKit/WebInspector.h>

@class WebViewDelegate;

using namespace ti;

@interface NativeWindow : NSWindow
{
	BOOL canReceiveFocus;
	WebView* webView;
	WebViewDelegate* delegate;
	BOOL requiresDisplay;
	AutoPtr<OSXUserWindow>* userWindow;
	WebInspector* inspector;
	BOOL fullscreen;
	BOOL focused;
	NSRect savedFrame;
}
- (void)setUserWindow:(AutoPtr<OSXUserWindow>*)inUserWindow;
- (void)setupDecorations
- (void)setTransparency:(double)transparency;
- (void)setFullscreen:(BOOL)yn;
- (void)close;
- (void)finishClose;
- (void)open;
- (void)frameLoaded;
- (WebView*)webView;
- (WindowConfig*)config;
- (UserWindow*)userWindow;
- (void)showInspector:(BOOL)console;
@end

#endif
