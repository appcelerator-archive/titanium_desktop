/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "../ui_module.h"
#include "osx_menu_item.h"
#include "WebFramePrivate.h"

@interface NSApplication (DeclarationStolenFromAppKit)
- (void)_cycleWindowsReversed:(BOOL)reversed;
@end

@implementation WebViewDelegate

- (void)setup 
{
	AppConfig *appConfig = AppConfig::Instance();
	std::string appid = appConfig->GetAppID();
	NSString *appID = [NSString stringWithCString:appid.c_str()];
	
	[webView setPreferencesIdentifier:appID];
	
	WebPreferences *webPrefs = [[WebPreferences alloc] initWithIdentifier:appID];
	// This indicates that WebViews in this app will not browse multiple pages, but rather show a small number.
	// this reduces memory cache footprint significantly.

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
	NSString *datadir = [NSString stringWithCString:kroll::FileUtils::GetApplicationDataDirectory(appid).c_str()];
	[webPrefs _setLocalStorageDatabasePath:datadir];
	
	[webPrefs setFullDocumentTeardownEnabled:YES];

	NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
	[standardUserDefaults setObject:datadir forKey:@"WebDatabaseDirectory"];
	[standardUserDefaults synchronize];
		
	[webView setPreferences:webPrefs];
	[webPrefs release];

	// this stuff adjusts the webview/window for chromeless windows.
	WindowConfig *o = [window config];
	
	if (o->IsUsingScrollbars())
	{
		[[[webView mainFrame] frameView] setAllowsScrolling:YES];
	}
	else
	{
		[[[webView mainFrame] frameView] setAllowsScrolling:NO];
	}
	if (o->IsResizable() && o->IsUsingChrome())
	{
		[window setShowsResizeIndicator:YES];
	}
	else
	{
		[window setShowsResizeIndicator:NO];
	}
	
	[webView setBackgroundColor:[NSColor clearColor]];
	[webView setAutoresizingMask:NSViewHeightSizable | NSViewWidthSizable];
	[webView setShouldCloseWithWindow:NO];
	
	// if we use a textured window mask, this is on by default which we don't want
	[window setMovableByWindowBackground:NO];
}

-(id)initWithWindow:(NativeWindow*)win host:(Host*)h
{
	self = [super init];
	if (self!=nil)
	{
		window = win;
		host = h;
		webView = [window webView];
		frames = new std::map<WebFrame*,SharedKObject>();
		[self setup];
		[webView setFrameLoadDelegate:self];
		[webView setUIDelegate:self];
		[webView setResourceLoadDelegate:self];
		[webView setPolicyDelegate:self];

		SharedKObject global = host->GetGlobalObject();
		const char* version = global->Get("version")->ToString();
		//TI-303 we need to add safari UA to our UA to resolve broken
		//sites that look at Safari and not WebKit for UA
		NSString *useragent = [NSString stringWithFormat:@"Version/4.0 Safari/528.16 %s/%s",PRODUCT_NAME,version];
		[webView setApplicationNameForUserAgent:useragent];
		// place our user agent string in the global so we can later use it
		const char *ua = [[webView userAgentForURL:[NSURL URLWithString:@"http://titaniumapp.com"]] UTF8String];
		global->Set("userAgent",Value::NewString(ua));
	}
	return self;
}

-(void)dealloc
{
	delete frames;
	[url release];
	[super dealloc];
}

-(void)show
{
	WindowConfig *config = [window config];
	config->SetVisible(true);
    [window makeKeyAndOrderFront:nil];	
}

- (NSURL *)url
{
    return url;
}

-(void)setURL:(NSURL*)newURL
{
	[url release];
	url = [newURL copy];
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

-(BOOL)newWindowAction:(NSDictionary*)actionInformation request:(NSURLRequest*)request listener:(id < WebPolicyDecisionListener >)listener
{
	NSDictionary* elementDict = [actionInformation objectForKey:WebActionElementKey];
	DOMNode *target = [elementDict objectForKey:WebElementDOMNodeKey];
	DOMElement *anchor = [self findAnchor:target];
	
	if (anchor)
	{
		NSString *target = [anchor getAttribute:@"target"];
		if (target)
		{
			if ([target isEqualToString:@"ti:systembrowser"])
			{
				NSURL *newURL = [request URL];
				[[NSWorkspace sharedWorkspace] openURL:newURL];
				[listener ignore];
				return NO;
			}
		}
	}

	NSString *protocol = [[actionInformation objectForKey:WebActionOriginalURLKey] scheme]; 
	NSURL *newURL = [request URL];
	if ([newURL isEqual:url])
	{
		[listener use];
		return NO;
	}
	
	if ([protocol isEqual:@"app"])
	{
		
		// if ([[TiController instance] shouldOpenInNewWindow])
		// {
		// 	// if we're trying to open an internal page, we essentially need to always open a 
		// 	// new document and later close the old document.  we have to do this because 
		// 	// each document could have a different window spec.
		// 	
		// 	TiDocument *doc = [[TiController instance] createDocument:newURL visible:YES config:nil];
		// 	[doc setPrecedent:self];
		// 	
		// 	//TODO: window opens slightly offset from current doc, make sure we 
		// 	//get the bounds from self and set on doc
		// 	[listener ignore];
		// }
		// else
		// {
		// 	// tell him to open in the same document and set our new URL
		// 	[self setURL:newURL];
		// 	[listener use];
		// }
		[self setURL:newURL];
		[listener use];
	}
	else if ([protocol isEqual:@"http"] || [protocol isEqual:@"https"])
	{
		// TODO: we need to probably make this configurable to support
		// opening the URL in the system browser (code below). for now 
		// we just open inside the same frame
		//[[NSWorkspace sharedWorkspace] openURL:newURL];
		[listener use];
	}
	return YES;
}

#pragma mark -
#pragma mark WebPolicyDelegate

- (void)webView:(WebView *)sender decidePolicyForNewWindowAction:(NSDictionary *)actionInformation request:(NSURLRequest *)request newFrameName:(NSString *)frameName decisionListener:(id < WebPolicyDecisionListener >)listener
{
	
	if (NO == [self newWindowAction:actionInformation request:request listener:listener])
	{
		return;
	}
	[listener ignore];
}

- (void)webView:(WebView *)sender decidePolicyForNavigationAction:(NSDictionary*) actionInformation request:(NSURLRequest*) request frame:(WebFrame*)frame decisionListener:(id <WebPolicyDecisionListener>)listener
{

	int type = [[actionInformation objectForKey:WebActionNavigationTypeKey] intValue];
	
	switch (type)
	{
		case WebNavigationTypeBackForward:
		case WebNavigationTypeReload:
		{
			[listener use];
			return;
		}
		case WebNavigationTypeLinkClicked:
		case WebNavigationTypeFormSubmitted:
		case WebNavigationTypeFormResubmitted:
		{
			break;
		}
		case WebNavigationTypeOther:
		{
			break;
		}
		default:
		{
			[listener ignore];
			return;
		}
	}
	NSString *protocol = [[actionInformation objectForKey:WebActionOriginalURLKey] scheme]; 
	NSURL *newURL = [request URL];
	if ([newURL isEqual:url])
	{
		[listener use];
		return ;
	}
	
	if ([protocol isEqual:@"app"] || [protocol isEqual:@"ti"])
	{
		// we only care about loading new TiDocuments if this is the main frame,
		// otherwise we're an internal frame of some kind
		if (frame != [[frame webView] mainFrame]) {
			[listener use];
			[self setURL:newURL];
			return;
		}
		[self setURL:newURL];
		[listener use];
	}
	else if ([protocol isEqual:@"http"] || [protocol isEqual:@"https"])
	{
		if (NO == [self newWindowAction:actionInformation request:request listener:listener])
		{
			return;
		}
		
		[self setURL:newURL];
		[listener use];
	}
	else
	{
		PRINTD("Application attempted to navigate to illegal location: " << [[newURL absoluteString] UTF8String]);
		[listener ignore];
	}
}

// WebFrameLoadDelegate Methods
#pragma mark -
#pragma mark WebFrameLoadDelegate

- (void)webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame
{
	(*frames)[frame]=SharedKObject(NULL);
}

- (void)webView:(WebView *)sender didReceiveTitle:(NSString *)title forFrame:(WebFrame *)frame
{
    // Only report feedback for the main frame.
	// set the title on the config in case they
	// programatically set the title on the window
	// so that it correctly is reflected in the config 
	WindowConfig *config = [window config];
	std::string t = std::string([title UTF8String]);
	config->SetTitle(t);
	[window setTitle:title];
}
- (SharedKObject)inject:(WebScriptObject *)windowScriptObject context:(JSGlobalContextRef)context frame:(WebFrame*)frame store:(BOOL)store
{
	
	UserWindow* userWindow = [window userWindow];
	userWindow->RegisterJSContext(context);

	// Track that we've cleared this frame
	JSObjectRef global_object = JSContextGetGlobalObject(context);
	KObject *global_bound_object = new KKJSObject(context, global_object);
	SharedKObject shared_global = global_bound_object;
	if (store)
	{
		(*frames)[frame] = shared_global;
	}
	return shared_global;
}

- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
	
	// we need to inject even in child frames
	std::map<WebFrame*,SharedKObject>::iterator iter = frames->find(frame);
	bool scriptCleared = false;
	SharedKObject global_object = SharedKObject(NULL);
	if (iter!=frames->end())
	{
		std::pair<WebFrame*,SharedKObject> pair = (*iter);
		global_object = pair.second;
		scriptCleared = global_object.get() != NULL;
		frames->erase(iter);
	}
	else
	{
		PRINTD("not found frame = " << frame);
	}

	JSGlobalContextRef context = [frame globalContext];
	if (!scriptCleared)
	{
		PRINTD("page loaded with no <script> tags, manually injecting Titanium runtime");
		// return the global object but don't store it since we're forcely
		// creating it since we need the global_object to be passed below
		global_object=[self inject:[frame windowObject] context:context frame:frame store:NO];
	}


	NSURL *theurl = [[[frame dataSource] request] URL];
	// fire load event
	UserWindow *user_window = [window userWindow];
	std::string url_str = [[theurl absoluteString] UTF8String];
	user_window->PageLoaded(global_object, url_str, context);
	
	if (![theurl isEqual:url])
	{
		[self setURL:theurl];
	}

	if (initialDisplay==NO)
	{
		initialDisplay=YES;
		// cause the initial window to show since it was initially opened hidden
		// so you don't get the nasty wide screen while content is loading
		[window performSelector:@selector(frameLoaded) withObject:nil afterDelay:.005];
	}
}

- (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{

	if ([error code]==-999 && [[error domain] isEqual:NSURLErrorDomain])
	{
		//this is OK, this is a cancel to a pending web load request and can be ignored...
		return;
	}

	Logger* logger = Logger::Get("UI.WebViewDelegate");
	std::string err = [[NSString stringWithFormat:@"Error loading URL: %@. %@", url,[error localizedDescription]] UTF8String];
	logger->Error(err);

	// in this case we need to ensure that the window is showing if not initially shown
	if (initialDisplay==NO)
	{
		initialDisplay=YES;
		[window performSelector:@selector(frameLoaded) withObject:nil afterDelay:.005];
	}
}

- (void)webView:(WebView *)sender didClearWindowObject:(WebScriptObject *)windowScriptObject forFrame:(WebFrame*)frame 
{
	JSGlobalContextRef context = [frame globalContext];
	[self inject:windowScriptObject context:context frame:frame store:YES];
}

// WebUIDelegate Methods
#pragma mark -
#pragma mark WebUIDelegate

- (WebView *)webView:(WebView *)sender createWebViewWithRequest:(NSURLRequest *)request
{
	// this is called when you attempt to create a new child window from this document
	// for example using window.open
	NSURL *newurl = [request URL];
	if (newurl==nil)
	{
		// this will be null in certain cases where the browser want's to call loadURL
		// on the new webview and he will pass nil .... just open a blank document
		// and return
		newurl = [NSURL URLWithString:@"about:blank"];
	}
	// TiDocument *newDoc = [[TiController instance] createDocument:newurl visible:YES config:nil];
	// [newDoc setPrecedent:self];
	// return [newDoc webView];
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
	PRINTD("webview_delegate::setFrame = "<<self);
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
    // Return some object that can be used to identify this resource
	// we just ignore this for now
	return nil;
}

-(NSURLRequest *)webView:(WebView *)sender resource:(id)identifier willSendRequest:(NSURLRequest *)request redirectResponse:(NSURLResponse *)redirectResponsefromDataSource:(WebDataSource *)dataSource
{
	PRINTD("webview_delegate::willSendRequest = "<<self);
    return request;
}

-(void)webView:(WebView *)sender resource:(id)identifier didFailLoadingWithError:(NSError *)error fromDataSource:(WebDataSource *)dataSource
{
	PRINTD("webview_delegate::didFailLoadingWithError = "<<[[error localizedDescription] UTF8String]);
}

-(void)webView:(WebView *)sender resource:(id)identifier didFinishLoadingFromDataSource:(WebDataSource *)dataSource
{
}

- (void)webView:(WebView *)wv runJavaScriptAlertPanelWithMessage:(NSString *)message initiatedByFrame:(WebFrame *)frame 
{
	PRINTD("alert = "<<[message UTF8String]);
	
	NSRunInformationalAlertPanel([window title],	// title
								 message,								// message
								 NSLocalizedString(@"OK", @""),			// default button
								 nil,									// alt button
								 nil);									// other button	

	// only show if already visible									
	if ([window userWindow]->IsVisible())
	{
		[window userWindow]->Show();
	}
}


- (BOOL)webView:(WebView *)wv runJavaScriptConfirmPanelWithMessage:(NSString *)message initiatedByFrame:(WebFrame *)frame 
{
	int result = NSRunInformationalAlertPanel([window title],	// title
													message,								// message
													NSLocalizedString(@"OK", @""),			// default button
													NSLocalizedString(@"Cancel", @""),		// alt button
													nil);
	// only show if already visible									
	if ([window userWindow]->IsVisible())
	{
		[window userWindow]->Show();
	}
	return NSAlertDefaultReturn == result;	
}


- (void)webView:(WebView *)wv runOpenPanelForFileButtonWithResultListener:(id <WebOpenPanelResultListener>)resultListener 
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
	[openPanel beginSheetForDirectory:nil 
								 file:nil 
					   modalForWindow:window
						modalDelegate:self
					   didEndSelector:@selector(openPanelDidEnd:returnCode:contextInfo:) 
						  contextInfo:resultListener];	
	// only show if already visible									
	if ([window userWindow]->IsVisible())
	{
		[window userWindow]->Show();
	}
	[window userWindow]->Show();
}


- (void)openPanelDidEnd:(NSSavePanel *)openPanel returnCode:(int)returnCode contextInfo:(void *)contextInfo 
{
	id <WebOpenPanelResultListener>resultListener = (id <WebOpenPanelResultListener>)contextInfo;
	// only show if already visible									
	if ([window userWindow]->IsVisible())
	{
		[window userWindow]->Show();
	}
	[window userWindow]->Show();
	if (NSOKButton == returnCode) {
		[resultListener chooseFilename:[openPanel filename]];
	}
}


- (BOOL)webView:(WebView *)wv shouldReplaceUploadFile:(NSString *)path usingGeneratedFilename:(NSString **)filename 
{
	return NO;
}


- (NSString *)webView:(WebView *)wv generateReplacementFile:(NSString *)path 
{
	PRINTD("generateReplacementFile: "<<[path UTF8String]);
	return nil;
}


- (BOOL)webView:(WebView *)wv shouldBeginDragForElement:(NSDictionary *)element dragImage:(NSImage *)dragImage mouseDownEvent:(NSEvent *)mouseDownEvent mouseDraggedEvent:(NSEvent *)mouseDraggedEvent 
{
	PRINTD("shouldBeginDragForElement");
	return YES;
}

//TODO: in 10.5, this becomes an NSUInteger
- (unsigned int)webView:(WebView *)wv dragDestinationActionMaskForDraggingInfo:(id <NSDraggingInfo>)draggingInfo 
{
	PRINTD("dragDestinationActionMaskForDraggingInfo");
	return WebDragDestinationActionAny;
}


- (void)webView:(WebView *)webView willPerformDragDestinationAction:(WebDragDestinationAction)action forDraggingInfo:(id <NSDraggingInfo>)draggingInfo 
{
	PRINTD("willPerformDragDestinationAction: "<<action);
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
	UserWindow *uw = [window userWindow];
	SharedPtr<MenuItem> menu = uw->GetContextMenu();
	NSMutableArray *array = [[[NSMutableArray alloc] init] autorelease];
	// window takes precedent - try him first
	if (menu.isNull())
	{
		// if no window, try the app context
		menu = UIModule::GetContextMenu();
	}
	if (!menu.isNull())
	{
		for (unsigned int c=0;c<menu->Size();c++)
		{
			SharedKObject item = menu->At(c)->ToObject();
			SharedPtr<OSXMenuItem> osx_menu = item.cast<OSXMenuItem>();
			NSMenuItem *native = osx_menu->CreateNative();
			[array addObject:native]; 
			[native release];
		}
	}
	return array;
}

#pragma mark -
#pragma mark WebScriptDebugDelegate

// some source was parsed, establishing a "source ID" (>= 0) for future reference
- (void)webView:(WebView *)webView       didParseSource:(NSString *)source
 baseLineNumber:(unsigned int)lineNumber
		fromURL:(NSURL *)aurl
	   sourceId:(int)sid
	forWebFrame:(WebFrame *)webFrame
{
}

// some source failed to parse
- (void)webView:(WebView *)webView  failedToParseSource:(NSString *)source
 baseLineNumber:(unsigned int)lineNumber
		fromURL:(NSURL *)theurl
	  withError:(NSError *)error
	forWebFrame:(WebFrame *)webFrame
{
	PRINTD("failed to parse javascript from "<<[[theurl absoluteString] UTF8String]<<" at lineNumber: " << lineNumber << ", error: " <<[[error localizedDescription] UTF8String]);
}

// just entered a stack frame (i.e. called a function, or started global scope)
- (void)webView:(WebView *)webView    didEnterCallFrame:(WebScriptCallFrame *)frame
	   sourceId:(int)sid
		   line:(int)lineno
	forWebFrame:(WebFrame *)webFrame
{
}

// about to execute some code
- (void)webView:(WebView *)webView willExecuteStatement:(WebScriptCallFrame *)frame
	   sourceId:(int)sid
		   line:(int)lineno
	forWebFrame:(WebFrame *)webFrame
{
}

// about to leave a stack frame (i.e. return from a function)
- (void)webView:(WebView *)webView   willLeaveCallFrame:(WebScriptCallFrame *)frame
	   sourceId:(int)sid
		   line:(int)lineno
	forWebFrame:(WebFrame *)webFrame
{
}

// exception is being thrown
- (void)webView:(WebView *)webView   exceptionWasRaised:(WebScriptCallFrame *)frame
	   sourceId:(int)sid
		   line:(int)lineno
	forWebFrame:(WebFrame *)webFrame
{
}

// return whether or not quota has been reached for a db (enabling db support)
- (void)webView:(WebView*)webView	frame:(WebFrame*)frame
	exceededDatabaseQuotaForSecurityOrigin:(id)origin
	database:(NSString*)dbName
{
	const unsigned long long defaultQuota = 5 * 1024 * 1024;
	
	[origin performSelector:@selector(setQuota:) withObject:[NSNumber numberWithInt:defaultQuota]];
}
@end


