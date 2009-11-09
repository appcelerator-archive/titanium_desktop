/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import "../ui_module.h"

namespace ti
{
	bool OSXUserWindow::initial = false;
	static unsigned int toWindowMask(WindowConfig *config)
	{
		unsigned int mask = 0;
		if (config->IsTexturedBackground())
		{
			mask |= NSTexturedBackgroundWindowMask;
		}
		if (!config->IsUsingChrome() || config->IsFullscreen())
		{
			return mask | NSBorderlessWindowMask;
		}
		else
		{
			return mask | NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask | NSMiniaturizableWindowMask;
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

		AutoPtr<OSXUserWindow>* shuw = new AutoPtr<OSXUserWindow>(this, true);
		[nativeWindow setUserWindow:shuw];

		if (!config->IsFullscreen())
		{
			NSRect rect = CalculateWindowFrame(
				config->GetX(), config->GetY(),
				config->GetWidth(), config->GetHeight());
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

		[nativeWindow setExcludedFromWindowsMenu:config->IsToolWindow()];

		[nativeWindow open];
		UserWindow::Open();
		this->FireEvent(Event::OPENED);
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
			[nativeWindow orderOut:nil];
			this->FireEvent(Event::HIDDEN);
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
			this->FireEvent(Event::SHOWN);
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

	bool OSXUserWindow::Close()
	{
		// Guard against re-closing a window
		if (!this->active || !this->nativeWindow)
			return false;

		UserWindow::Close();

		// If the window is still active at this point, it  indicates
		// an event listener has cancelled this close event.
		if (!this->active)
		{
			// Actually close the native window and
			// mark this UserWindow as invalid.
			this->Closed();
			[nativeWindow finishClose];
		}

		return !this->active;
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
		y = (screenFrame.size.height - (height + y)) + screenFrame.origin.y;

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
			NSRect newRect = CalculateWindowFrame(
				x, this->GetY(), this->GetWidth(), this->GetHeight());
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
			NSRect newRect = CalculateWindowFrame(
				this->GetX(), y, this->GetWidth(), this->GetHeight());
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
			NSRect newFrame = CalculateWindowFrame(
				this->GetX(), this->GetY(), width, this->GetHeight());

			// We only want to change the width
			newFrame.size.height = [nativeWindow frame].size.height;

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
			NSRect newFrame = CalculateWindowFrame(
				this->GetX(), this->GetY(), this->GetWidth(), height);

			// We only want to change the height
			newFrame.size.width = [nativeWindow frame].size.width;

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
		double maxWidth = this->config->GetMaxWidth();
		double minWidth = this->config->GetMinWidth();
		double maxHeight = this->config->GetMaxHeight();
		double minHeight = this->config->GetMinHeight();

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
			NSRect newFrame = CalculateWindowFrame(
				bounds.x, bounds.y, bounds.width, bounds.height);

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

	void OSXUserWindow::SetTitleImpl(std::string& newTitle)
	{
		if (active && nativeWindow != nil)
		{
			[nativeWindow setTitle:[NSString stringWithUTF8String:newTitle.c_str()]];
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
			std::string nurl = kroll::URLUtils::NormalizeURL(url);
			NSURL* nsurl = [NSURL URLWithString: [NSString stringWithUTF8String:nurl.c_str()]];
			[[[nativeWindow webView] mainFrame] loadRequest:[NSURLRequest requestWithURL:nsurl]];
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
		if (nativeWindow != nil)
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
		if (nativeWindow != nil)
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
		if (nativeWindow != nil)
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
		if (nativeWindow != nil)
		{
			[nativeWindow setTransparency:transparency];
		}
	}

	void OSXUserWindow::SetFullscreen(bool fullscreen)
	{
		if (nativeWindow != nil)
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
		this->osxBinding->WindowFocused(AutoPtr<OSXUserWindow>(this, true));
	}

	void OSXUserWindow::Unfocused()
	{
		this->osxBinding->WindowUnfocused(AutoPtr<OSXUserWindow>(this, true));
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
		this->iconPath = iconPath;
	}

	std::string& OSXUserWindow::GetIcon()
	{
		return this->iconPath;
	}

	bool OSXUserWindow::IsTopMost()
	{
		return this->config->IsTopMost();
	}

	void OSXUserWindow::SetTopMost(bool topmost)
	{
		if (nativeWindow != nil)
		{
			if (topmost)
			{
				[nativeWindow setLevel:NSPopUpMenuWindowLevel];
			}
			else
			{
				[nativeWindow setLevel:NSNormalWindowLevel];
			}
		}
	}

	void OSXUserWindow::OpenChooserDialog(
		bool files,
		KMethodRef callback,
		bool multiple,
		std::string& title,
		std::string& path,
		std::string& defaultName,
		std::vector<std::string>& types,
		std::string& typesDescription)
	{
		KListRef results = new StaticBoundList();
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
			filename = [NSString stringWithUTF8String:defaultName.c_str()];
		}
		if (!path.empty())
		{
			begin = [NSString stringWithUTF8String:path.c_str()];
		}
		if (types.size() > 0)
		{
			filetypes = [[NSMutableArray alloc] init];
			for (size_t t = 0; t < types.size(); t++)
			{
				const char *s = types.at(t).c_str();
				[filetypes addObject:[NSString stringWithUTF8String:s]];
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
		KMethodRef callback,
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
		KMethodRef callback,
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
		KMethodRef callback,
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
			[filetypes addObject:[NSString stringWithUTF8String:ft.c_str()]];
		}

		NSSavePanel *sp = [NSSavePanel savePanel];
		[sp setTitle:[NSString stringWithUTF8String:title.c_str()]];

		if ([filetypes count] > 0)
		{
			[sp setAllowedFileTypes:filetypes];
		}

		runResult = [sp 
			runModalForDirectory:[NSString stringWithUTF8String:path.c_str()]
			file:[NSString stringWithUTF8String:defaultName.c_str()]];

		ValueList args;

		KListRef results = new StaticBoundList();
		if (runResult == NSFileHandlingPanelOKButton) 
		{
			NSString *selected = [sp filename];
			results->Append(Value::NewString([selected UTF8String]));
		}

		callback->Call(ValueList(Value::NewList(results)));
		this->Show();
	}
	
	void OSXUserWindow::ShowInspector(bool console)
	{
		[nativeWindow showInspector:console];
	}
}
    
