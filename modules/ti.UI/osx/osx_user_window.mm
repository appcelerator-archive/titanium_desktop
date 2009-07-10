/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import "../ui_module.h"
#define STUB() printf("Method is still a stub, %s:%i\n", __FILE__, __LINE__)

namespace ti
{
	bool OSXUserWindow::initial = false;
	static unsigned int toWindowMask(WindowConfig *config)
	{
		if (!config->IsUsingChrome() || config->IsFullscreen())
		{
			return NSBorderlessWindowMask | NSTexturedBackgroundWindowMask ;
		}
		else
		{
			return NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask | NSMiniaturizableWindowMask | NSTexturedBackgroundWindowMask;
		}
	}

	OSXUserWindow::OSXUserWindow(WindowConfig* config, AutoUserWindow& parent) :
		UserWindow(config, parent),
		nativeWindow(nil),
		menu(0),
		contextMenu(0),
		osxBinding(binding.cast<OSXUIBinding>())
	{
		// Initialization of the native window and its properties now happen in Open(),
		// so that developers can tweak window properties before comitting to them
		// by calling Open(...)
	}

	void OSXUserWindow::Open()
	{
		unsigned int mask = toWindowMask(config);
		NSRect mainFrame = [[NSScreen mainScreen] frame];
		NSRect frame = mainFrame;

		// Set up the size and position of the
		// window using our Set<...> methods so
		// we avoid duplicating the logic here.
		if (!config->IsFullscreen())
			frame = NSMakeRect(0, 0, 10, 10);

		nativeWindow = [[NativeWindow alloc]
			initWithContentRect: frame
			styleMask: mask
			backing: NSBackingStoreBuffered
			defer: false];

		AutoPtr<OSXUserWindow>* shuw = new AutoPtr<OSXUserWindow>(0);
		(*shuw) = shared_this.cast<OSXUserWindow>();
		[nativeWindow setUserWindow:shuw];

		if (!config->IsFullscreen())
		{
			this->real_x = config->GetX();
			this->real_y = config->GetY();
			this->real_w = config->GetWidth();
			this->real_h = config->GetHeight();
			NSRect rect = CalculateWindowFrame(real_x, real_y, real_w, real_h);
			[nativeWindow setFrame:rect display:NO animate:NO];

			this->ReconfigureWindowConstraints();
			if (!config->IsResizable())
			{
				[nativeWindow setMinSize: rect.size];
				[nativeWindow setMaxSize: rect.size];
			}
		}

		this->SetCloseable(config->IsCloseable());
		this->SetMaximizable(config->IsMaximizable());
		this->SetMinimizable(config->IsMinimizable());

		[nativeWindow setupDecorations:config];
		if (OSXUserWindow::initial)
		{
			OSXUserWindow::initial = false;
			[nativeWindow setInitialWindow:YES];
		}

		this->SetTopMost(config->IsTopMost());

		if (config->IsMaximized())
		{
			[nativeWindow zoom:nativeWindow];
		}

		if (config->IsMinimized())
		{
			[nativeWindow miniaturize:nativeWindow];
		}

		[nativeWindow open];
		UserWindow::Open();
		this->FireEvent(OPENED);
	}

	OSXUserWindow::~OSXUserWindow()
	{
		this->Close();
		[nativeWindow dealloc];
	}

	void OSXUserWindow::Hide()
	{
		if (active && nativeWindow)
		{
			this->Unfocus();
			this->FireEvent(HIDDEN);
		}
	}

	void OSXUserWindow::Focus()
	{
		if (active && nativeWindow && ![nativeWindow isKeyWindow])
		{
			[nativeWindow makeKeyAndOrderFront:nil];
			this->Focused();
		}
	}

	void OSXUserWindow::Unfocus()
	{
		// Cocoa doesn't really have a concept of blurring a window, but
		// we can send the window to the back of the window list.
		if (active && nativeWindow && [nativeWindow isKeyWindow])
		{
			[nativeWindow orderBack:nil];
			this->Unfocused();
		}
	}

	void OSXUserWindow::Show()
	{
		if (active && nativeWindow)
		{
			this->Focus();
			this->FireEvent(SHOWN);
		}
	}

	void OSXUserWindow::Minimize()
	{
		if (active && nativeWindow)
		{
			[nativeWindow miniaturize:nativeWindow];
		}
	}

	void OSXUserWindow::Unminimize()
	{
		if (active && nativeWindow && [nativeWindow isMiniaturized])
		{
			[nativeWindow deminiaturize:nativeWindow];
		}
	}

	bool OSXUserWindow::IsMinimized()
	{
		if (active && nativeWindow)
		{
			return [nativeWindow isMiniaturized];
		}
		else
		{
			return this->config->IsMinimized();
		}
	}

	void OSXUserWindow::Maximize()
	{
		if (active && nativeWindow)
		{
			[nativeWindow zoom:nativeWindow];
		}
	}
	
	void OSXUserWindow::Unmaximize()
	{
		if (active && nativeWindow && [nativeWindow isZoomed])
		{
			[nativeWindow zoom:nativeWindow];
		}
	}

	bool OSXUserWindow::IsMaximized()
	{
		if (active && nativeWindow)
		{
			return [nativeWindow isZoomed];
		}
		else
		{
			return this->config->IsMinimized();
		}
	}

	bool OSXUserWindow::IsUsingChrome()
	{
		return this->config->IsUsingChrome();
	}

	bool OSXUserWindow::IsUsingScrollbars()
	{
		return this->config->IsUsingScrollbars();
	}

	bool OSXUserWindow::IsFullscreen()
	{
		return this->config->IsFullscreen();
	}

	std::string OSXUserWindow::GetId()
	{
		return this->config->GetID();
	}

	void OSXUserWindow::Close()
	{
		if (active && nativeWindow) { // Do not re-close
			UserWindow::Close();
			this->Closed();

			// Actually close the native window and mark
			// this UserWindow as invalid.
			[nativeWindow finishClose];
		}
	}

	NSScreen* OSXUserWindow::GetWindowScreen()
	{
		NSScreen* screen = [nativeWindow screen];
		if (screen == nil) 
		{
			// Window is offscreen, so set things relative to the main screen.
			// The other option in this case would be to use the "closest" screen,
			// which might be better, but the real fix is to add support for multiple
			// screens in the UI API.
			screen = [NSScreen mainScreen];	

		}
		return screen;
	}

	NSRect OSXUserWindow::CalculateWindowFrame(double x, double y, double width, double height)
	{
		NSRect frame = [nativeWindow frame];
		NSRect contentFrame = [[nativeWindow contentView] frame];
		NSRect screenFrame = [this->GetWindowScreen() frame];
		
		// Center frame, if requested
		if (y == UIBinding::CENTERED)
		{
			y = (screenFrame.size.height - height) / 2;
			config->SetY(y);
		}
		if (x == UIBinding::CENTERED)
		{
			x = (screenFrame.size.width - width) / 2;
			config->SetX(x);
		}

		// Now we adjust for the size of the frame decorations
		width += frame.size.width - contentFrame.size.width;
		height += frame.size.height - contentFrame.size.height;

		// Adjust the position for the origin of this screen and use cartesian coordinates
		x += screenFrame.origin.x;
		y = screenFrame.origin.y + (screenFrame.size.height - (y + height));

		return NSMakeRect(x, y, width, height);
	}

	double OSXUserWindow::GetX()
	{
		if (active && nativeWindow)
		{
			// Cocoa frame coordinates are absolute on a plane with all
			// screens, but Titanium wants them relative to the screen.
			NSRect screenFrame = [this->GetWindowScreen() frame];
			return [nativeWindow frame].origin.x - screenFrame.origin.x;
		}
		else
		{
			return this->config->GetX();
		}
	}
	
	void OSXUserWindow::SetX(double x)
	{
		if (active && nativeWindow)
		{
			this->real_x = x; // Preserve input value
			NSRect newRect = CalculateWindowFrame(x, real_y, real_w, real_h);
			[nativeWindow setFrameOrigin: newRect.origin];
		}
	}

	double OSXUserWindow::GetY()
	{
		if (active && nativeWindow)
		{
			// Cocoa frame coordinates are absolute on a plane with all
			// screens, but Titanium wants them relative to the screen.
			NSRect screenFrame = [this->GetWindowScreen() frame];
			double y = [nativeWindow frame].origin.y - screenFrame.origin.y;

			// Adjust for the cartesian coordinate system
			y = screenFrame.size.height - y - [nativeWindow frame].size.height;
			return y;
		}
		else
		{
			return this->config->GetX();
		}
	}

	void OSXUserWindow::SetY(double y)
	{
		if (active && nativeWindow)
		{
			this->real_y = y; // Preserve input value
			NSRect newRect = CalculateWindowFrame(real_x, real_y, real_w, real_h);
			[nativeWindow setFrameOrigin: newRect.origin];
		}
	}

	double OSXUserWindow::GetWidth()
	{
		if (active && nativeWindow)
		{
			return [[nativeWindow contentView] frame].size.width;
		}
		else
		{
			return this->config->GetWidth();
		}
	}

	void OSXUserWindow::SetWidth(double width)
	{
		if (active && nativeWindow)
		{
			this->real_w = width;
			NSRect newFrame = CalculateWindowFrame(real_x, real_y, width, real_h);

			if (!config->IsResizable())
			{
				[nativeWindow setMinSize: newFrame.size];
				[nativeWindow setMaxSize: newFrame.size];
			}
			[nativeWindow setFrame:newFrame display:config->IsVisible() animate:YES];
		}
	}

	double OSXUserWindow::GetHeight()
	{
		if (active && nativeWindow)
		{
			return [[nativeWindow contentView] frame].size.height;
		}
		else
		{
			return this->config->GetHeight();
		}
	}

	void OSXUserWindow::SetHeight(double height)
	{
		if (active && nativeWindow)
		{
			this->real_h = height;
			NSRect newFrame = CalculateWindowFrame(real_x, real_y, real_w, real_h);

			if (!config->IsResizable())
			{
				[nativeWindow setMinSize: newFrame.size];
				[nativeWindow setMaxSize: newFrame.size];
			}
			[nativeWindow setFrame:newFrame display:config->IsVisible() animate:NO];
		}
	}

	void OSXUserWindow::ReconfigureWindowConstraints()
	{
		if (!active || nativeWindow == nil)
		{
			return;
		}

		NSSize minSize, maxSize;
		double maxWidth = (int) this->config->GetMaxWidth();
		double minWidth = (int) this->config->GetMinWidth();
		double maxHeight = (int) this->config->GetMaxHeight();
		double minHeight = (int) this->config->GetMinHeight();

		if (maxWidth == -1)
		{
			maxSize.width = FLT_MAX;
		}
		else
		{
			maxSize.width = maxWidth;
		}

		if (minWidth == -1)
		{
			minSize.width = 1;
		}
		else
		{
			minSize.width = minWidth;
		}

		if (maxHeight == -1)
		{
			maxSize.height = FLT_MAX;
		}
		else
		{
			maxSize.height = maxHeight;
		}

		if (minHeight == -1)
		{
			minSize.height = 1;
		}
		else
		{
			minSize.height = minHeight;
		}

		[nativeWindow setContentMinSize:minSize];
		[nativeWindow setContentMaxSize:maxSize];
	}

	double OSXUserWindow::GetMaxWidth()
	{
		return this->config->GetMaxWidth();
	}

	void OSXUserWindow::SetMaxWidth(double width)
	{
		this->ReconfigureWindowConstraints();
	}

	double OSXUserWindow::GetMinWidth()
	{
		return this->config->GetMinWidth();
	}

	void OSXUserWindow::SetMinWidth(double width)
	{
		this->ReconfigureWindowConstraints();
	}

	double OSXUserWindow::GetMaxHeight()
	{
		return this->config->GetMaxHeight();
	}

	void OSXUserWindow::SetMaxHeight(double height)
	{
		this->ReconfigureWindowConstraints();
	}
	
	double OSXUserWindow::GetMinHeight()
	{
		return this->config->GetMinHeight();
	}
	
	void OSXUserWindow::SetMinHeight(double height)
	{
		this->ReconfigureWindowConstraints();
	}

	Bounds OSXUserWindow::GetBounds()
	{
		Bounds b;
		b.width = this->GetWidth();
		b.height = this->GetHeight();
		b.x = this->GetX();
		b.y = this->GetY();
		return b;
	}

	void OSXUserWindow::SetBounds(Bounds bounds)
	{
		if (active && nativeWindow)
		{
			this->real_x = bounds.x;
			this->real_y = bounds.y;
			this->real_w = bounds.width;
			this->real_h = bounds.height;
			NSRect newFrame = CalculateWindowFrame(real_x, real_y, real_w, real_h);

			if (!config->IsResizable())
			{
				[nativeWindow setMinSize: newFrame.size];
				[nativeWindow setMaxSize: newFrame.size];
			}
			[nativeWindow setFrame:newFrame display:config->IsVisible() animate:YES];
		}
	}

	std::string OSXUserWindow::GetTitle()
	{
		return this->config->GetTitle();
	}

	void OSXUserWindow::SetTitle(std::string& title)
	{
		if (active && nativeWindow != nil)
		{
			[nativeWindow setTitle:[NSString stringWithCString:this->config->GetTitle().c_str() encoding:NSUTF8StringEncoding]];
		}
	}

	std::string OSXUserWindow::GetURL()
	{
		if (active && nativeWindow) {
			NSString* url = [[nativeWindow webView] mainFrameURL];
			return [url UTF8String];
		} else {
			return this->config->GetURL();
		}
	}

	void OSXUserWindow::SetURL(std::string& url)
	{
		if (active && nativeWindow != nil)
		{
			std::string url_str = AppConfig::Instance()->InsertAppIDIntoURL(config->GetURL());
			NSURL* url = [NSURL URLWithString: [NSString stringWithCString:url_str.c_str() encoding:NSUTF8StringEncoding]];
			[[[nativeWindow webView] mainFrame] loadRequest:[NSURLRequest requestWithURL:url]];
		}
	}

	bool OSXUserWindow::IsResizable()
	{
		return this->config->IsResizable();
	}

	void OSXUserWindow::SetResizable(bool resizable)
	{
		if (active && nativeWindow != nil)
		{
			[nativeWindow setShowsResizeIndicator:resizable];
			if (resizable)
			{
				[nativeWindow setContentMinSize: NSMakeSize(config->GetMinWidth(), config->GetMinHeight())];
				[nativeWindow setContentMaxSize: NSMakeSize(config->GetMaxWidth(), config->GetMaxHeight())];
			}
			else
			{
				[nativeWindow setMinSize: [nativeWindow frame].size];
				[nativeWindow setMaxSize: [nativeWindow frame].size];
			}
		}
	}

	bool OSXUserWindow::IsMaximizable()
	{
		return this->config->IsMaximizable();
	}

	void OSXUserWindow::SetMaximizable(bool maximizable)
	{
		if (active && nativeWindow != nil)
		{
			[[nativeWindow standardWindowButton:NSWindowZoomButton] setHidden:!maximizable];
		}
	}

	bool OSXUserWindow::IsMinimizable()
	{
		return this->config->IsMinimizable();
	}

	void OSXUserWindow::SetMinimizable(bool minimizable)
	{
		if (active && nativeWindow != nil)
		{
			[[nativeWindow standardWindowButton:NSWindowMiniaturizeButton] setHidden:!minimizable];
		}
	}

	bool OSXUserWindow::IsCloseable()
	{
		return this->config->IsCloseable();
	}

	void OSXUserWindow::SetCloseable(bool closeable)
	{
		if (active && nativeWindow != nil)
		{
			[[nativeWindow standardWindowButton:NSWindowCloseButton] setHidden:!closeable];
		}
	}

	bool OSXUserWindow::IsVisible()
	{
		return this->config->IsVisible();
	}

	double OSXUserWindow::GetTransparency()
	{
		return this->config->GetTransparency();
	}

	void OSXUserWindow::SetTransparency(double transparency)
	{
		if (active && nativeWindow != nil)
		{
			[nativeWindow setTransparency:transparency];
		}
	}

	void OSXUserWindow::SetFullscreen(bool fullscreen)
	{
		if (active && nativeWindow != nil)
		{
			[nativeWindow setFullscreen:fullscreen];
		}
	}

	void OSXUserWindow::SetUsingChrome(bool chrome)
	{
		this->config->SetUsingChrome(chrome);
	}

	void OSXUserWindow::SetMenu(AutoMenu menu)
	{	
		if (this->menu.get() == menu.get())
		{
			return;
		}
		this->menu = menu.cast<OSXMenu>();
		osxBinding->SetupMainMenu();
	}

	AutoMenu OSXUserWindow::GetMenu()
	{
		return this->menu;
	}

	void OSXUserWindow::Focused()
	{
		AutoPtr<OSXUserWindow> osxWin = this->shared_this.cast<OSXUserWindow>();
		this->osxBinding->WindowFocused(osxWin);
	}

	void OSXUserWindow::Unfocused()
	{
		AutoPtr<OSXUserWindow> osxWin = this->shared_this.cast<OSXUserWindow>();
		this->osxBinding->WindowUnfocused(osxWin);
	}
	
	void OSXUserWindow::SetContextMenu(AutoMenu menu)
	{
		this->contextMenu = menu.cast<OSXMenu>();
	}

	AutoMenu OSXUserWindow::GetContextMenu()
	{
		return this->contextMenu;
	}

	void OSXUserWindow::SetIcon(std::string& iconPath)
	{
		STUB();
	}

	std::string& OSXUserWindow::GetIcon()
	{
		STUB();
		static std::string stubby = "";
		return stubby;
	}

	bool OSXUserWindow::IsTopMost()
	{
		return this->topmost;
	}

	void OSXUserWindow::SetTopMost(bool topmost)
	{
		if (active && nativeWindow != nil)
		{
			if (topmost)
			{
				[nativeWindow setLevel:NSPopUpMenuWindowLevel];
				this->topmost = true;
			}
			else
			{
				[nativeWindow setLevel:NSNormalWindowLevel];
				this->topmost = false;
			}
		}
	}

	void OSXUserWindow::OpenChooserDialog(
		bool files,
		SharedKMethod callback,
		bool multiple,
		std::string& title,
		std::string& path,
		std::string& defaultName,
		std::vector<std::string>& types,
		std::string& typesDescription)
	{
		SharedKList results = new StaticBoundList();
		NSOpenPanel* openDlg = [NSOpenPanel openPanel];
		[openDlg setTitle:[NSString stringWithUTF8String:title.c_str()]];
		[openDlg setCanChooseFiles:files];
		[openDlg setCanChooseDirectories:!files];
		[openDlg setAllowsMultipleSelection:multiple];
		[openDlg setResolvesAliases:YES];

		NSMutableArray *filetypes = nil;
		NSString *begin = nil, *filename = nil;

		if (!defaultName.empty())
		{
			filename = [NSString stringWithCString:defaultName.c_str() encoding:NSUTF8StringEncoding];
		}
		if (!path.empty())
		{
			begin = [NSString stringWithCString:path.c_str() encoding:NSUTF8StringEncoding];
		}
		if (types.size() > 0)
		{
			filetypes = [[NSMutableArray alloc] init];
			for (size_t t = 0; t < types.size(); t++)
			{
				const char *s = types.at(t).c_str();
				[filetypes addObject:[NSString stringWithCString:s encoding:NSUTF8StringEncoding]];
			}
		}

		if ([openDlg runModalForDirectory:begin file:filename types:filetypes] == NSOKButton)
		{
			NSArray* selected = [openDlg filenames];
			for (int i = 0; i < (int)[selected count]; i++)
			{
				NSString* fileName = [selected objectAtIndex:i];
				std::string fn = [fileName UTF8String];
				results->Append(Value::NewString(fn));
			}
		}
		[filetypes release];

		callback->Call(ValueList(Value::NewList(results)));
		this->Show();

	}

	void OSXUserWindow::OpenFileChooserDialog(
		SharedKMethod callback,
		bool multiple,
		std::string& title,
		std::string& path,
		std::string& defaultName,
		std::vector<std::string>& types,
		std::string& typesDescription)
	{
		this->OpenChooserDialog(
			true, callback, multiple,
			title, path, defaultName, types, typesDescription);
	}

	void OSXUserWindow::OpenFolderChooserDialog(
		SharedKMethod callback,
		bool multiple,
		std::string& title,
		std::string& path,
		std::string& defaultName)
	{
		std::vector<std::string> types;
		std::string typesDescription;
		this->OpenChooserDialog(
			false, callback, multiple,
			title, path, defaultName, types, typesDescription);
	}

	void OSXUserWindow::OpenSaveAsDialog(
		SharedKMethod callback,
		std::string& title,
		std::string& path,
		std::string& defaultName,
		std::vector<std::string>& types,
		std::string& typesDescription)
	{
		int runResult;

		NSMutableArray *filetypes = [[NSMutableArray alloc] init];
		std::vector<std::string>::const_iterator iter = types.begin();
		while (iter != types.end())
		{
			std::string ft = (*iter++);
			[filetypes addObject:[NSString stringWithCString:ft.c_str() encoding:NSUTF8StringEncoding]];
		}

		NSSavePanel *sp = [NSSavePanel savePanel];
		[sp setTitle:[NSString stringWithUTF8String:title.c_str()]];

		if ([filetypes count] > 0)
		{
			[sp setAllowedFileTypes:filetypes];
		}

		runResult = [sp runModalForDirectory:[NSString stringWithCString:path.c_str() encoding:NSUTF8StringEncoding] file:[NSString stringWithCString:defaultName.c_str() encoding:NSUTF8StringEncoding]];

		ValueList args;

		SharedKList results = new StaticBoundList();
		if (runResult == NSFileHandlingPanelOKButton) 
		{
			NSString *selected = [sp filename];
			results->Append(Value::NewString([selected UTF8String]));
		}

		callback->Call(ValueList(Value::NewList(results)));
		this->Show();
	}
}
    
