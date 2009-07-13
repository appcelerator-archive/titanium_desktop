/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "../ui_module.h"
#include "osx_menu_item.h"
#include <WebKit/WebFramePrivate.h>
#include <WebKit/WebPreferenceKeysPrivate.h>

@interface NSApplication (DeclarationStolenFromAppKit)
- (void)_cycleWindowsReversed:(BOOL)reversed;
@end

@implementation WebViewDelegate

-(void)setupPreferences
{
	AppConfig *appConfig = AppConfig::Instance();
	std::string appid = appConfig->GetAppID();
	NSString *appID = [NSString stringWithUTF8String:appid.c_str()];
	[[window webView] setPreferencesIdentifier:appID];

	WebPreferences *webPrefs = [[WebPreferences alloc] initWithIdentifier:appID];

	// This indicates that WebViews in this app will not browse multiple pages,
	// but rather show a small number. This reduces memory cache footprint
	// significantly.
	[webPrefs setCacheModel:WebCacheModelDocumentBrowser];

	[webPrefs setDeveloperExtrasEnabled:host->IsDebugMode()];
	[webPrefs setPlugInsEnabled:YES]; 
	[webPrefs setJavaEnabled:YES];
	[webPrefs setJavaScriptEnabled:YES];

	if ([webPrefs respondsToSelector:@selector(setDatabasesEnabled:)])
	{
		[webPrefs setDatabasesEnabled:YES];
	}
	if ([webPrefs respondsToSelector:@selector(setLocalStorageEnabled:)])
	{
		[webPrefs setLocalStorageEnabled:YES];
	}
	[webPrefs setDOMPasteAllowed:YES];
	[webPrefs setUserStyleSheetEnabled:NO];

	// Setup the DB to store it's DB under our data directory for the app
	NSString *datadir = [NSString stringWithUTF8String:
		kroll::FileUtils::GetApplicationDataDirectory(appid).c_str()];
	[webPrefs _setLocalStorageDatabasePath:datadir];

	NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
	[standardUserDefaults
		setObject:[NSNumber numberWithInt:1]
		forKey:WebKitEnableFullDocumentTeardownPreferenceKey];
	[standardUserDefaults
		setObject:datadir
		forKey:@"WebDatabaseDirectory"];
	[standardUserDefaults synchronize];

	[[window webView] setPreferences:webPrefs];
	[webPrefs release];

}

-(id)initWithWindow:(NativeWindow*)inWindow
{
	self = [super init];
	if (self == nil)
		return self;

	window = inWindow;
	logger = Logger::Get("UI.WebViewDelegate");
	frameToGlobalObject = new std::map<WebFrame*, SharedKObject>();
	host = Host::GetInstance();
	WebView* webView = [window webView];

	[self setupPreferences];

	// This stuff adjusts the webview/window for chromeless windows.
	WindowConfig *o = [window config];
	if (o->IsUsingScrollbars()) {
		[[[webView mainFrame] frameView] setAllowsScrolling:YES];
	} else {
		[[[webView mainFrame] frameView] setAllowsScrolling:NO];
	}
	if (o->IsResizable() && o->IsUsingChrome()) {
		[window setShowsResizeIndicator:YES];
	} else {
		[window setShowsResizeIndicator:NO];
	}

	[webView setBackgroundColor:[NSColor clearColor]];
	[webView setAutoresizingMask:NSViewHeightSizable | NSViewWidthSizable];
	[webView setShouldCloseWithWindow:NO];

	// if we use a textured window mask, this is on by default which we don't want
	[window setMovableByWindowBackground:NO];

	// TI-303 we need to add safari UA to our UA to resolve broken
	// sites that look at Safari and not WebKit for UA
	NSString *appName = [NSString
		stringWithFormat:@"Version/4.0 Safari/528.16 %s/%s",
		PRODUCT_NAME, STRING(PRODUCT_VERSION)];
	[webView setApplicationNameForUserAgent:appName];

	// place our user agent string in the global so we can later use it
	SharedKObject global = host->GetGlobalObject();
	NSString* fullUserAgent = [webView userAgentForURL:
		[NSURL URLWithString:@"http://titaniumapp.com"]];
	global->SetString("userAgent", [fullUserAgent UTF8String]);

	return self;
}

-(void)dealloc
{
	delete frameToGlobalObject;
	[super dealloc];
}

-(void)show
{
	WindowConfig *config = [window config];
	config->SetVisible(true);
	[window makeKeyAndOrderFront:nil];
}

-(DOMElement*)findAnchor:(DOMNode*)node
{
	while (node)
	{
		if ([node nodeType] == 1 && [[node nodeName] isEqualToString:@"A"])
		{
			return (DOMElement*)node;
		}
		node = [node parentNode];
	}
	return nil;
}

#pragma mark -
#pragma mark WebPolicyDelegate

- (void)webView:(WebView *)sender decidePolicyForNewWindowAction:(NSDictionary *)actionInformation request:(NSURLRequest *)request newFrameName:(NSString *)frameName decisionListener:(id < WebPolicyDecisionListener >)listener
{
	NSString *protocol = 
		[[actionInformation objectForKey:WebActionOriginalURLKey] scheme]; 

	if (frameName &&
		([frameName isEqualToString:@"ti:systembrowser"] ||
		[frameName isEqualToString:@"_blank"]))
	{
		NSURL *newURL = [request URL];
		[[NSWorkspace sharedWorkspace] openURL:newURL];
		[listener ignore];
	}
	else if ([[request URL] isEqual:[sender mainFrameURL]])
	{
		[listener use];
	}
	else if ([protocol isEqual:@"http"] || [protocol isEqual:@"https"])
	{
		[listener use];
	}
	else
	{
		[listener ignore];
	}
}

- (void)webView:(WebView *)sender decidePolicyForNavigationAction:(NSDictionary*) actionInformation request:(NSURLRequest*) request frame:(WebFrame*)frame decisionListener:(id <WebPolicyDecisionListener>)listener
{
	[listener use];
	//int type = [[actionInformation objectForKey:WebActionNavigationTypeKey] intValue];
	//
	//switch (type)
	//{
	//	case WebNavigationTypeBackForward:
	//	case WebNavigationTypeReload:
	//	{
	//		[listener use];
	//		return;
	//	}
	//	case WebNavigationTypeLinkClicked:
	//	case WebNavigationTypeFormSubmitted:
	//	case WebNavigationTypeFormResubmitted:
	//	{
	//		break;
	//	}
	//	case WebNavigationTypeOther:
	//	{
	//		break;
	//	}
	//	default:
	//	{
	//		[listener ignore];
	//		return;
	//	}
	//}
	//NSString *protocol = 
	//	[[actionInformation objectForKey:WebActionOriginalURLKey] scheme]; 
	//if ([protocol isEqual:@"app"] || [protocol isEqual:@"ti"] ||
	//	[protocol isEqual:@"http"] || [protocol isEqual:@"https"])
	//{
	//	[listener use];
	//}
	//else
	//{
	//	logger->Warn("Application attempted to navigate to illegal location: %s",
	//		[[newURL absoluteString] UTF8String]);
	//	[listener ignore];
	//}
}

// WebFrameLoadDelegate Methods
#pragma mark -
#pragma mark WebFrameLoadDelegate

- (void)registerGlobalObject:(SharedKObject)globalObject forFrame:(WebFrame *)frame
{
	(*frameToGlobalObject)[frame] = globalObject;
}

- (SharedKObject)registerJSContext:(JSGlobalContextRef)context forFrame:(WebFrame*)frame
{
	UserWindow* userWindow = [window userWindow];
	userWindow->RegisterJSContext(context);

	// Track that we've cleared this frame
	JSObjectRef globalObject = JSContextGetGlobalObject(context);
	SharedKObject globalKObject  = new KKJSObject(context, globalObject);
	[self registerGlobalObject:globalKObject forFrame:frame];

	return globalKObject;
}

- (BOOL)isGlobalObjectRegisteredForFrame:(WebFrame*) frame
{
	std::map<WebFrame*, SharedKObject>::iterator iter =
		frameToGlobalObject->find(frame);
	return iter != frameToGlobalObject->end();
}

- (SharedKObject)globalObjectForFrame:(WebFrame*) frame
{
	std::map<WebFrame*, SharedKObject>::iterator iter =
		frameToGlobalObject->find(frame);
	if (iter == frameToGlobalObject->end()) {
		return NULL;
	} else {
		return iter->second;
	}
}

- (void)deregisterGlobalObjectForFrame:(WebFrame *)frame
{
	std::map<WebFrame*, SharedKObject>::iterator i =
		frameToGlobalObject->find(frame);
	frameToGlobalObject->erase(frame);
}

- (void)webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame
{
	// If this NULL value is registered when the load finishes, we need
	// to manually inject Titanium into the frame. if the frame isn't in
	// the map at all -- an error happened and we shouldn't inject Titanium.
	[self registerGlobalObject:NULL forFrame:frame];
}

- (void)webView:(WebView *)senderwillCloseFrame:(WebFrame *)frame
{
	[self deregisterGlobalObjectForFrame:frame];
}

- (void)webView:(WebView *)sender didReceiveTitle:(NSString *)title forFrame:(WebFrame *)frame
{
}

- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{

	if (![self isGlobalObjectRegisteredForFrame:frame]) {
		// This was a failed load, so do not continue
		return;
	}

	JSGlobalContextRef context = [frame globalContext];
	SharedKObject global = [self globalObjectForFrame:frame];
	if (global.isNull()) {
		// The load was successful, but this page doesn't have a script tag
		global = [self registerJSContext:context forFrame:frame];
	}

	// fire load event
	UserWindow *userWindow = [window userWindow];
	std::string url = [[[[[frame dataSource] request] URL] absoluteString] UTF8String];
	userWindow->PageLoaded(global, url, context);

	if (!initialDisplay)
	{
		initialDisplay = YES;
		// cause the initial window to show since it was initially opened hidden
		// so you don't get the nasty white screen while content is loading
		[window performSelector:@selector(frameLoaded) withObject:nil afterDelay:.005];
	}
}

- (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{
	// The fact that this frame is missing from our list or registered
	// global contexts means that the frame load failed and we shouldn't
	// register this context or fire the page loaded message.
	[self deregisterGlobalObjectForFrame:frame];

	NSString* urlString = [[[[frame dataSource] request] URL] absoluteString];
	logger->Error("didFailProvisionalLoadWithError (%s): %s",
		[urlString UTF8String], [[error localizedDescription] UTF8String]);

	// in this case we need to ensure that the window is showing if not initially shown
	if (!initialDisplay)
	{
		initialDisplay = YES;
		[window performSelector:@selector(frameLoaded) withObject:nil afterDelay:.005];
	}
}

- (void)webView:(WebView *)sender didClearWindowObject:(WebScriptObject *)windowScriptObject forFrame:(WebFrame*)frame 
{
	JSGlobalContextRef context = [frame globalContext];
	[self registerJSContext:context forFrame:frame];
}

// WebUIDelegate Methods
#pragma mark -
#pragma mark WebUIDelegate

- (WebView *)webView:(WebView *)sender createWebViewWithRequest:(NSURLRequest *)request
{
	return nil;
}

- (void)webViewShow:(WebView *)sender
{
}

- (void)webViewClose:(WebView *)wv 
{
	[[wv window] close];
	WindowConfig *config = [window config];
	config->SetVisible(NO);
	
	if (inspector)
	{
		[inspector webViewClosed];
	}
}

- (void)webViewFocus:(WebView *)wv 
{
	[[wv window] makeKeyAndOrderFront:wv];
}

- (void)webViewUnfocus:(WebView *)wv 
{
	if ([[wv window] isKeyWindow] || [[[wv window] attachedSheet] isKeyWindow]) 
	{
		[NSApp _cycleWindowsReversed:FALSE];
	}
}

- (NSResponder *)webViewFirstResponder:(WebView *)wv 
{
	return [[wv window] firstResponder];
}

- (void)webView:(WebView *)wv makeFirstResponder:(NSResponder *)responder 
{
	[[wv window] makeFirstResponder:responder];
}

- (NSString *)webViewStatusText:(WebView *)wv 
{
	return nil;
}

- (BOOL)webViewIsResizable:(WebView *)wv 
{
	WindowConfig *config = [window config];
	return config->IsResizable();
}

- (void)webView:(WebView *)wv setResizable:(BOOL)resizable; 
{
	WindowConfig *config = [window config];
	config->SetResizable(resizable);
	[[wv window] setShowsResizeIndicator:resizable];
}


- (void)webView:(WebView *)wv setFrame:(NSRect)frame 
{
	[[wv window] setFrame:frame display:YES];
}

- (NSRect)webViewFrame:(WebView *)wv 
{
	NSWindow *w = [wv window];
	return w == nil ? NSZeroRect : [w frame];
}

- (BOOL)webViewAreToolbarsVisible:(WebView *)wv 
{
	return NO;
}

- (BOOL)webViewIsStatusBarVisible:(WebView *)wv 
{
	return NO;
}

// WebResourceLoadDelegate Methods
#pragma mark -
#pragma mark WebResourceLoadDelegate

- (id)webView:(WebView *)sender identifierForInitialRequest:(NSURLRequest *)request fromDataSource:(WebDataSource *)dataSource
{
	// Return some object that can be used to identify
	// this resource. We just ignore this for now.
	return nil;
}

-(NSURLRequest *)webView:(WebView *)sender resource:(id)identifier willSendRequest:(NSURLRequest *)request redirectResponse:(NSURLResponse *)redirectResponsefromDataSource:(WebDataSource *)dataSource
{
	return request;
}

-(void)webView:(WebView *)sender resource:(id)identifier didFailLoadingWithError:(NSError *)error fromDataSource:(WebDataSource *)dataSource
{
	logger->Error("didFailLoadingWithError: %s",
		[[error localizedDescription] UTF8String]);
}

-(void)webView:(WebView *)sender resource:(id)identifier didFinishLoadingFromDataSource:(WebDataSource *)dataSource
{
}

- (void)webView:(WebView *)wv runJavaScriptAlertPanelWithMessage:(NSString *)message initiatedByFrame:(WebFrame *)frame 
{
	NSRunInformationalAlertPanel([window title], // title
		message, // message
		NSLocalizedString(@"OK", @""), // default button
		nil, // alt button
		nil); // other button

	// only show if already visible
	if ([window userWindow]->IsVisible())
	{
		[window userWindow]->Show();
	}
}


- (BOOL)webView:(WebView *)wv runJavaScriptConfirmPanelWithMessage:(NSString *)message initiatedByFrame:(WebFrame *)frame 
{
	int result = NSRunInformationalAlertPanel([window title], // title
		message, // message
		NSLocalizedString(@"OK", @""), // default button
		NSLocalizedString(@"Cancel", @""), // alt button
		nil);

	// only show if already visible
	if ([window userWindow]->IsVisible())
	{
		[window userWindow]->Show();
	}
	return NSAlertDefaultReturn == result;
}


- (void)webView:(WebView *)sender runOpenPanelForFileButtonWithResultListener:(id<WebOpenPanelResultListener>)resultListener;
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
	[openPanel setAllowsMultipleSelection:NO];
	[openPanel setCanChooseDirectories:NO];
	[openPanel beginSheetForDirectory:nil
		file:nil 
		modalForWindow:window
		modalDelegate:self
		didEndSelector:@selector(openPanelDidEnd:returnCode:contextInfo:) 
		contextInfo:resultListener];
	[openPanel retain];
	[resultListener retain];
}

- (void)webView:(WebView *)sender runOpenPanelForFileButtonWithResultListener:(id<WebOpenPanelResultListener>)resultListener allowMultipleFiles:(BOOL)allowMultipleFiles 
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
	[openPanel setAllowsMultipleSelection:allowMultipleFiles];
	[openPanel setCanChooseDirectories:NO];
	[openPanel beginSheetForDirectory:nil
		file:nil 
		modalForWindow:window
		modalDelegate:self
		didEndSelector:@selector(openPanelDidEnd:returnCode:contextInfo:) 
		contextInfo:resultListener];
	[openPanel retain];
	[resultListener retain];
}

- (void)openPanelDidEnd:(NSOpenPanel *)openPanel returnCode:(int)returnCode contextInfo:(void *)contextInfo 
{
	id <WebOpenPanelResultListener>resultListener = (id <WebOpenPanelResultListener>)contextInfo;

	if (NSOKButton == returnCode) {
		[resultListener chooseFilenames:[openPanel filenames]];
		[resultListener autorelease];
		[openPanel autorelease];
	}
}


- (BOOL)webView:(WebView *)wv shouldReplaceUploadFile:(NSString *)path usingGeneratedFilename:(NSString **)filename 
{
	return NO;
}


- (NSString *)webView:(WebView *)wv generateReplacementFile:(NSString *)path 
{
	return nil;
}


- (BOOL)webView:(WebView *)wv shouldBeginDragForElement:(NSDictionary *)element dragImage:(NSImage *)dragImage mouseDownEvent:(NSEvent *)mouseDownEvent mouseDraggedEvent:(NSEvent *)mouseDraggedEvent 
{
	return YES;
}

//TODO: in 10.5, this becomes an NSUInteger
- (unsigned int)webView:(WebView *)wv dragDestinationActionMaskForDraggingInfo:(id <NSDraggingInfo>)draggingInfo 
{
	return WebDragDestinationActionAny;
}


- (void)webView:(WebView *)webView willPerformDragDestinationAction:(WebDragDestinationAction)action forDraggingInfo:(id <NSDraggingInfo>)draggingInfo 
{
	// NSPasteboard *pasteboard = [draggingInfo draggingPasteboard];
	// PRINTD("pasteboard types: %@",[pasteboard types]);
}


//TODO: in 10.5, this becomes an NSUInteger
- (unsigned int)webView:(WebView *)wv dragSourceActionMaskForPoint:(NSPoint)point
{
	return WebDragSourceActionAny;
}

- (NSArray *)webView:(WebView *)sender contextMenuItemsForElement:(NSDictionary *)element defaultMenuItems:(NSArray *)defaultMenuItems
{
	NSMutableArray *menuItems = [[[NSMutableArray alloc] init] autorelease];

	UserWindow *uw = [window userWindow];
	AutoPtr<OSXMenu> menu = uw->GetContextMenu().cast<OSXMenu>();
	if (menu.isNull()) {
		menu = UIBinding::GetInstance()->GetContextMenu().cast<OSXMenu>();
	}

	if (!menu.isNull()) {
		menu->AddChildrenToNSArray(menuItems);
	}
	return menuItems;
}

#pragma mark -
#pragma mark WebScriptDebugDelegate

// some source was parsed, establishing a "source ID" (>= 0) for future reference
- (void)webView:(WebView *)webView
	didParseSource:(NSString *)source
	baseLineNumber:(unsigned int)lineNumber
	fromURL:(NSURL *)aurl
	sourceId:(int)sid
	forWebFrame:(WebFrame *)webFrame
{
}

// some source failed to parse
- (void)webView:(WebView *)webView
	failedToParseSource:(NSString *)source
	baseLineNumber:(unsigned int)lineNumber
	fromURL:(NSURL *)theurl
	withError:(NSError *)error
	forWebFrame:(WebFrame *)webFrame
{
	logger->Error("Failed to parse javascript from %s at line number %i: %s",
		[[theurl absoluteString] UTF8String],
		lineNumber,
		[[error localizedDescription] UTF8String]);
}

// just entered a stack frame (i.e. called a function, or started global scope)
- (void)webView:(WebView *)webView
	didEnterCallFrame:(WebScriptCallFrame *)frame
	sourceId:(int)sid
	line:(int)lineno
	forWebFrame:(WebFrame *)webFrame
{
}

// about to execute some code
- (void)webView:(WebView *)webView
	willExecuteStatement:(WebScriptCallFrame *)frame
	sourceId:(int)sid
	line:(int)lineno
	forWebFrame:(WebFrame *)webFrame
{
}

// about to leave a stack frame (i.e. return from a function)
- (void)webView:(WebView *)webView
	willLeaveCallFrame:(WebScriptCallFrame *)frame
	sourceId:(int)sid
	line:(int)lineno
	forWebFrame:(WebFrame *)webFrame
{
}

// exception is being thrown
- (void)webView:(WebView *)webView
	exceptionWasRaised:(WebScriptCallFrame *)frame
	sourceId:(int)sid
	line:(int)lineno
	forWebFrame:(WebFrame *)webFrame
{
}

// return whether or not quota has been reached for a db (enabling db support)
- (void)webView:(WebView*)webView
	frame:(WebFrame*)frame
	exceededDatabaseQuotaForSecurityOrigin:(id)origin
	database:(NSString*)dbName
{
	const unsigned long long defaultQuota = 5 * 1024 * 1024;
	[origin performSelector:@selector(setQuota:) withObject:[NSNumber numberWithInt:defaultQuota]];
}
@end


