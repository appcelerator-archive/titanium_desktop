/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"

@implementation NativeWindow
- (BOOL)canBecomeKeyWindow
{
	return YES;
}

- (BOOL)canBecomeMainWindow
{
	return YES;
}

- (void)setUserWindow:(AutoPtr<OSXUserWindow>*)inUserWindow
{
	userWindow = inUserWindow;
}

- (void)setupDecorations:(WindowConfig*)inConfig;
{
	config = inConfig;

	[self setTitle:[NSString stringWithUTF8String:config->GetTitle().c_str()]];
	[self setOpaque:false];
	[self setHasShadow:true];

	webView = [[WebView alloc] init];
	[webView setDrawsBackground:NO];

	delegate = [[WebViewDelegate alloc] initWithWindow:self];
	[webView setFrameLoadDelegate:delegate];
	[webView setUIDelegate:delegate];
	[webView setResourceLoadDelegate:delegate];
	[webView setPolicyDelegate:delegate];

	[self setContentView:webView];
	[self setDelegate:self];
	[self setTransparency:config->GetTransparency()];
	[self setInitialFirstResponder:webView];
	[self setShowsResizeIndicator:config->IsResizable()];
}

- (void)dealloc
{
	// make sure we go back to normal mode
	SetSystemUIMode(kUIModeNormal, 0);

	[webView setFrameLoadDelegate:nil];
	[webView setUIDelegate:nil];
	[webView setResourceLoadDelegate:nil];
	[webView setPolicyDelegate:nil];

	[delegate release];
	delegate = nil;

	[inspector release];
	[webView release];
	webView = nil;
	[super dealloc];
}

- (UserWindow*)userWindow
{
	return userWindow->get();
}

- (void)showInspector:(id)sender
{
	if (inspector == nil)
	{
		inspector = [[WebInspector alloc] initWithWebView:webView];
		[inspector detach:self];
	}
	[inspector show:self];
}

- (void)titaniumQuit:(id)sender
{
	Host* host = Host::GetInstance();
	host->Exit(0);
}

- (void)windowWillClose:(NSNotification *)notification
{
	if (inspector)
	{
		[inspector close:self];
		[inspector release];
		inspector = nil;
	}
	config->SetVisible(false);
}

- (NSSize)windowWillResize:(NSWindow *) window toSize:(NSSize)newSize
{
	return newSize;
}

- (void)windowDidResize:(NSNotification*)notification
{
	(*userWindow)->FireEvent(Event::RESIZED);
}

- (void)windowDidMove:(NSNotification*)notification
{
	(*userWindow)->FireEvent(Event::MOVED);
}

- (void)windowDidBecomeKey:(NSNotification*)notification
{
	(*userWindow)->FireEvent(Event::FOCUSED);
	(*userWindow)->Focused();
	if (!focused && fullscreen)
	{
		SetSystemUIMode(kUIModeAllHidden, kUIOptionAutoShowMenuBar);
	}
	focused = YES;
}

- (void)windowDidResignKey:(NSNotification*)notification
{
	(*userWindow)->FireEvent(Event::UNFOCUSED);
	(*userWindow)->Unfocused();
	if (fullscreen && focused)
	{
		SetSystemUIMode(kUIModeNormal, 0);
	}
	focused = NO;
}

- (void)windowDidMiniaturize:(NSNotification*)notification
{
	(*userWindow)->FireEvent(Event::MINIMIZED);
}

- (BOOL)windowShouldZoom:(NSWindow*)window toFrame:(NSRect)proposedFrame
{
	(*userWindow)->FireEvent(Event::MAXIMIZED);
	return YES;
}

- (void)setTransparency:(double)transparency
{
	[self setAlphaValue:transparency];
	if (transparency < 1.0)
	{
		[webView setBackgroundColor:[NSColor clearColor]];
	}
	else
	{
		[webView setBackgroundColor:[NSColor whiteColor]];
	}
	[self invalidateShadow];
}

- (NSScreen *)activeScreen
{
	NSArray *screens = [NSScreen screens];
	/* if we've only got one screen then return it */
	if ([screens count] <= 1) 
	{
		return [NSScreen mainScreen];
	}
	
	NSScreen* screen = [self deepestScreen];
	if (screen != nil)
	{
		return screen;
	}
	
	return [NSScreen mainScreen];
}

- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen 
{
	if (fullscreen)
	{
		return frameRect;
	}
	return [super constrainFrameRect:frameRect toScreen:screen];
}

- (void)setFullscreen:(BOOL)yn
{
	if (yn)
	{
		fullscreen = YES;
		savedFrame = [self frame];
		
		// adjust to remove toolbar from view on window
		NSView *toolbarView = [[self toolbar] valueForKey:@"toolbarView"];
		float toolbarHeight = [toolbarView frame].size.height;
		if (![[self toolbar] isVisible]) {
			toolbarHeight = 0;
		}
		float windowBarHeight = [self frame].size.height - ([[self contentView] frame].size.height + toolbarHeight);
		NSRect frame = [[NSScreen mainScreen] frame];
		frame.size.height += windowBarHeight;

		SetSystemUIMode(kUIModeAllHidden,kUIOptionAutoShowMenuBar);
		[self setFrame:frame display:YES animate:YES];
		(*userWindow)->FireEvent(Event::FULLSCREENED);
		[self setShowsResizeIndicator:NO];
	}
	else
	{
		fullscreen = NO;
		[self setFrame:savedFrame display:YES animate:YES];
		SetSystemUIMode(kUIModeNormal,0);
		[self setShowsResizeIndicator:config->IsResizable()];
		(*userWindow)->FireEvent(Event::UNFULLSCREENED);
	}
	[self makeKeyAndOrderFront:nil];
	[self makeFirstResponder:webView];
}

- (WebView*)webView
{
	return webView;
}

- (WindowConfig*)config
{
	return config;
}

- (void)open
{
	if (config->IsVisible() && !config->IsMinimized())
	{
		// if we call open and we're initially visible
		// we need to basically set requires display which
		// will cause the window to be shown once the url is loaded
		requiresDisplay = YES;
	}
	std::string url_str = AppConfig::Instance()->InsertAppIDIntoURL(config->GetURL());
	NSURL* url = [NSURL URLWithString: [NSString stringWithCString:url_str.c_str() encoding:NSUTF8StringEncoding]];
	[[webView mainFrame] loadRequest:[NSURLRequest requestWithURL:url]];
}

- (void)close
{
	(*userWindow)->Close();
}

- (void)finishClose
{
	[webView close];
	[super close];
}

- (void)setInitialWindow:(BOOL)yn
{
	// this is a boolean to indicate that when the frame is loaded,
	// we should go ahead and display the window
	requiresDisplay = yn;
}

- (void)frameLoaded
{
	if (requiresDisplay)
	{
		requiresDisplay = NO;
		config->SetVisible(true);

		[NSApp arrangeInFront:self];
		[self makeKeyAndOrderFront:self];
		[NSApp activateIgnoringOtherApps:YES];

		if (config->IsFullscreen())
		{
			[self setFullscreen:YES];
		}
	}
	[self invalidateShadow];
}
@end
