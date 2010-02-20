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

- (void)setupDecorations:(AutoPtr<WindowConfig>)config
{
	[self setTitle:[NSString stringWithUTF8String:config->GetTitle().c_str()]];
	[self setHasShadow:true];

	webView = [[WebView alloc] initWithFrame:[[self contentView] bounds]];

	delegate = [[WebViewDelegate alloc] initWithWindow:self];
	[webView setFrameLoadDelegate:delegate];
	[webView setUIDelegate:delegate];
	[webView setResourceLoadDelegate:delegate];
	[webView setPolicyDelegate:delegate];

	// Because of the Win32 implementation, this setting
	// only applies during window creation.
	if (config->HasTransparentBackground())
	{
		[self setOpaque:false];
		[self setBackgroundColor:[NSColor clearColor]];
		[webView setDrawsBackground:NO];
		[webView setBackgroundColor:[NSColor clearColor]];
	}
	else if (config->HasTexturedBackground())
	{
		[self setOpaque:false];
		[webView setDrawsBackground:NO];
		[self setMovableByWindowBackground:NO]; // On by default.

		// HACK: When a large portion of a textured window is covered
		// by an opaque subview, OS X will push the window gradient up
		// to emulate the look of a non-textured window. Since we are
		// *really* transparent here, make the alpha value less than
		// 1.0 to trick OS X into knowing that we are transparent.
		[webView setAlphaValue:0.99];
	}
	else
	{
		// This is to match the default Windows behavior of painting
		// the background of the web view white.
		[webView setBackgroundColor:[NSColor whiteColor]];
	}

	if (config->IsResizable() && config->IsUsingChrome())
		[self setShowsResizeIndicator:YES];
	else
		[self setShowsResizeIndicator:NO];

	[self setDelegate:self];
	[self setTransparency:config->GetTransparency()];
	[self setInitialFirstResponder:webView];
	[self setContentView:webView];
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

- (OSXUserWindow*)userWindow
{
	return userWindow->get();
}
- (void)showInspector:(BOOL)console
{
	if (inspector == nil)
	{
		inspector = [[WebInspector alloc] initWithWebView:webView];
		[inspector detach:webView];
	}
	
	if (console)
	{
		[inspector showConsole:webView];
	}
	else
	{
		[inspector show:webView];
	}
}

- (void)titaniumQuit:(id)sender
{
	Host::GetInstance()->Exit(0);
}

- (void)windowWillClose:(NSNotification *)notification
{
	if (inspector)
	{
		[inspector close:self];
		[inspector release];
		inspector = nil;
	}
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
	return frameRect;
}

- (void)setFullscreen:(BOOL)yn
{
	// if we're not already visible, don't cause setfullscreen to necessarily
	// cause us to become visible -- this gives us more fine grain control that 
	// separates hide/show from fullscreen/unfullscreened
	BOOL display = (*userWindow)->IsVisible() && [self isVisible]; 
	if (yn)
	{
		fullscreen = YES;
		savedFrame = [self frame];

		float toolbarHeight = 0;
		if ([[self toolbar] isVisible])
			toolbarHeight = [[[self toolbar] valueForKey:@"toolbarView"]
				frame].size.height;

		NSRect frame = [[NSScreen mainScreen] frame];
		frame.size.height += [self frame].size.height -
			([[self contentView] frame].size.height + toolbarHeight);

		SetSystemUIMode(kUIModeAllHidden,kUIOptionAutoShowMenuBar);
		[self setFrame:frame display:display animate:display];
		(*userWindow)->FireEvent(Event::FULLSCREENED);
		[self setShowsResizeIndicator:NO];
	}
	else
	{
		fullscreen = NO;
		[self setFrame:savedFrame display:display animate:display];
		SetSystemUIMode(kUIModeNormal,0);
		[self setShowsResizeIndicator:(*userWindow)->IsResizable()];
		(*userWindow)->FireEvent(Event::UNFULLSCREENED);
	}
	if (display)
	{
		[self makeKeyAndOrderFront:nil];
		[self makeFirstResponder:webView];
	}
}

- (WebView*)webView
{
	return webView;
}

- (void)open
{
	// Wait until the frame has loaded to show the window. This prevents
	// showing an unloaded WebView (a white screen).
	requiresDisplay = YES;

	if ((*userWindow)->IsFullscreen())
		[self setFullscreen:YES];
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

- (void)frameLoaded
{
	if (requiresDisplay && (*userWindow)->IsVisible())
	{
		requiresDisplay = NO;
		(*userWindow)->Show();
	}
	[self invalidateShadow];
}

-(NSRect)windowWillUseStandardFrame:(NSWindow *)sender defaultFrame:(NSRect)defaultFrame
{
	return [[self screen] frame];
}

@end
