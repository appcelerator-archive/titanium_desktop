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

#ifndef NativeWindow_h
#define NativeWindow_h

#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

#include "../../ti.App/WindowConfig.h"

namespace Titanium {

class UserWindowMac;

} // namespace Titanium

@class WebViewDelegate;

@interface NativeWindow : NSWindow <NSWindowDelegate>
{
    BOOL canReceiveFocus;
    WebView* webView;
    WebViewDelegate* delegate;
    BOOL requiresDisplay;
    AutoPtr<Titanium::UserWindowMac>* userWindow;
    BOOL fullscreen;
    BOOL focused;
    NSRect savedFrame;
}
- (void)setUserWindow:(AutoPtr<Titanium::UserWindowMac>*)inUserWindow;
- (void)setupDecorations:(AutoPtr<Titanium::WindowConfig>)config;
- (void)setTransparency:(double)transparency;
- (void)setFullscreen:(BOOL)yn;
- (void)close;
- (void)finishClose;
- (void)open;
- (void)frameLoaded;
- (WebView*)webView;
- (Titanium::UserWindowMac*)userWindow;
- (void)showInspector:(BOOL)console;
@end

#endif
