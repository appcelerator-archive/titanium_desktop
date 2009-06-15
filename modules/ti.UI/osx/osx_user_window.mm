/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#import "../ui_module.h"
#import "osx_menu_item.h"

#define STUB() printf("Method is still a stub, %s:%i\n", __FILE__, __LINE__)

namespace ti
{
	bool OSXUserWindow::initial = false;
	static unsigned int toWindowMask(WindowConfig *config)
	{
		if (!config->IsUsingChrome() || config->IsFullScreen())
		{
			return NSBorderlessWindowMask | NSTexturedBackgroundWindowMask ;
		}
		else
		{
			return NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask | NSMiniaturizableWindowMask | NSTexturedBackgroundWindowMask;
		}
	}

	OSXUserWindow::OSXUserWindow(WindowConfig* config, SharedUserWindow& parent) :
		UserWindow(config, parent),
		window(nil),
		opened(false),
		closed(false),
		osx_binding(binding.cast<OSXUIBinding>())
	{
		// Initialization of the window and its properties now happen in Open(),
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
		if (!config->IsFullScreen())
			frame = NSMakeRect(0, 0, 10, 10);

		window = [[NativeWindow alloc]
			initWithContentRect: frame
			styleMask: mask
			backing: NSBackingStoreBuffered
			defer: false];

		if (!config->IsFullScreen())
		{
			this->real_x = config->GetX();
			this->real_y = config->GetY();
			this->real_w = config->GetWidth();
			this->real_h = config->GetHeight();
			NSRect rect = CalculateWindowFrame(real_x, real_y, real_w, real_h);
			[window setFrame:rect display:NO animate:NO];

			this->ReconfigureWindowConstraints();
			if (!config->IsResizable())
			{
				[window setMinSize: rect.size];
				[window setMaxSize: rect.size];
			}
		}

		this->SetCloseable(config->IsCloseable());
		this->SetMaximizable(config->IsMaximizable());
		this->SetMinimizable(config->IsMinimizable());

		[window setupDecorations:config host:binding->GetHost() userwindow:this];
		if (OSXUserWindow::initial)
		{
			OSXUserWindow::initial = false;
			[window setInitialWindow:YES];
		}

		this->SetTopMost(config->IsTopMost());

		if (config->IsMaximized())
		{
			[window zoom:window];
		}

		if (config->IsMinimized())
		{
			[window miniaturize:window];
		}

		opened = true;
		[window open];
		UserWindow::Open();
	}

	OSXUserWindow::~OSXUserWindow()
	{
		UserWindow::Close();
	}

	void OSXUserWindow::Hide()
	{
		if (window != nil)
		{
			this->Unfocus();
			[window fireWindowEvent:HIDDEN];
		}
	}

	void OSXUserWindow::Focus()
	{
		if (window != nil && ![window isKeyWindow])
		{
			[window makeKeyAndOrderFront:nil];
			this->Focused();
		}
	}

	void OSXUserWindow::Unfocus()
	{
		// Cocoa doesn't really have a concept of blurring a window, but
		// we can send the window to the back of the window list.
		if (window != nil && [window isKeyWindow])
		{
			[window orderBack:nil];
			this->Unfocused();
		}
	}

	void OSXUserWindow::Show()
	{
		if (window != nil)
		{
			this->Focus();
			[window fireWindowEvent:SHOWN];
		}
	}

	void OSXUserWindow::Minimize()
	{
		if (window != nil)
		{
			[window miniaturize:window];
		}
	}

	void OSXUserWindow::Unminimize()
	{
		if (window != nil && [window isMiniaturized])
		{
			[window deminiaturize:window];
		}
	}

	bool OSXUserWindow::IsMinimized()
	{
		if (window != nil)
		{
			return [window isMiniaturized];
		}
		else
		{
			return this->config->IsMinimized();
		}
	}

	void OSXUserWindow::Maximize()
	{
		if (window != nil)
		{
			[window zoom:window];
		}
	}
	
	void OSXUserWindow::Unmaximize()
	{
		if (window != nil && [window isZoomed])
		{
			[window zoom:window];
		}
	}

	bool OSXUserWindow::IsMaximized()
	{
		if (window != nil)
		{
			return [window isZoomed];
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

	bool OSXUserWindow::IsFullScreen()
	{
		return this->config->IsFullScreen();
	}

	std::string OSXUserWindow::GetId()
	{
		return this->config->GetID();
	}

	void OSXUserWindow::Close()
	{
		if (window != nil && !closed)
		{
			opened = false;
			closed = true;
			[window close];
			window = nil; // don't release
			UserWindow::Close();
		}
	}

	NSScreen* OSXUserWindow::GetWindowScreen()
	{

		NSScreen* screen = [window screen];
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
		NSRect frame = [window frame];
		NSRect contentFrame = [[window contentView] frame];
		NSRect screenFrame = [this->GetWindowScreen() frame];
		
		// Center frame, if requested
		if (y == UserWindow::CENTERED)
		{
			y = (screenFrame.size.height - height) / 2;
			config->SetY(y);
		}
		if (x == UserWindow::CENTERED)
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
		if (window != nil)
		{
			// Cocoa frame coordinates are absolute on a plane with all
			// screens, but Titanium wants them relative to the screen.
			NSRect screenFrame = [this->GetWindowScreen() frame];
			return [window frame].origin.x - screenFrame.origin.x;
		}
		else
		{
			return this->config->GetX();
		}
	}
	
	void OSXUserWindow::SetX(double x)
	{
		if (window != nil)
		{
			this->real_x = x; // Preserve input value
			NSRect newRect = CalculateWindowFrame(x, real_y, real_w, real_h);
			[window setFrameOrigin: newRect.origin];
		}
	}

	double OSXUserWindow::GetY()
	{
		if (window != nil)
		{
			// Cocoa frame coordinates are absolute on a plane with all
			// screens, but Titanium wants them relative to the screen.
			NSRect screenFrame = [this->GetWindowScreen() frame];
			double y = [window frame].origin.y - screenFrame.origin.y;

			// Adjust for the cartesian coordinate system
			y = screenFrame.size.height - y - [window frame].size.height;
			return y;
		}
		else
		{
			return this->config->GetX();
		}
	}

	void OSXUserWindow::SetY(double y)
	{
		if (window != nil)
		{
			this->real_y = y; // Preserve input value
			NSRect newRect = CalculateWindowFrame(real_x, real_y, real_w, real_h);
			[window setFrameOrigin: newRect.origin];
		}
	}

	double OSXUserWindow::GetWidth()
	{
		if (window != nil)
		{
			return [[window contentView] frame].size.width;
		}
		else
		{
			return this->config->GetWidth();
		}
	}

	void OSXUserWindow::SetWidth(double width)
	{
		if (window != nil)
		{
			this->real_w = width;
			NSRect newFrame = CalculateWindowFrame(real_x, real_y, width, real_h);

			if (!config->IsResizable())
			{
				[window setMinSize: newFrame.size];
				[window setMaxSize: newFrame.size];
			}
			[window setFrame:newFrame display:config->IsVisible() animate:YES];
		}
	}

	double OSXUserWindow::GetHeight()
	{
		if (window != nil)
		{
			return [[window contentView] frame].size.height;
		}
		else
		{
			return this->config->GetHeight();
		}
	}

	void OSXUserWindow::SetHeight(double height)
	{
		if (window != nil)
		{
			this->real_h = height;
			NSRect newFrame = CalculateWindowFrame(real_x, real_y, real_w, real_h);

			if (!config->IsResizable())
			{
				[window setMinSize: newFrame.size];
				[window setMaxSize: newFrame.size];
			}
			[window setFrame:newFrame display:config->IsVisible() animate:NO];
		}
	}

	void OSXUserWindow::ReconfigureWindowConstraints()
	{
		if (window == nil)
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

		[window setContentMinSize:minSize];
		[window setContentMaxSize:maxSize];
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
		if (window != nil)
		{
			this->real_x = bounds.x;
			this->real_y = bounds.y;
			this->real_w = bounds.width;
			this->real_h = bounds.height;
			NSRect newFrame = CalculateWindowFrame(real_x, real_y, real_w, real_h);

			if (!config->IsResizable())
			{
				[window setMinSize: newFrame.size];
				[window setMaxSize: newFrame.size];
			}
			[window setFrame:newFrame display:config->IsVisible() animate:YES];
		}
	}

	std::string OSXUserWindow::GetTitle()
	{
		return this->config->GetTitle();
	}

	void OSXUserWindow::SetTitle(std::string& title)
	{
		if (window != nil)
		{
			[window setTitle:[NSString stringWithCString:this->config->GetTitle().c_str()]];
		}
	}

	std::string OSXUserWindow::GetURL()
	{
		return this->config->GetURL();
	}

	void OSXUserWindow::SetURL(std::string& url)
	{
		if (window != nil)
		{
			std::string url_str = AppConfig::Instance()->InsertAppIDIntoURL(config->GetURL());
			NSURL* url = [NSURL URLWithString: [NSString stringWithCString:url_str.c_str()]];
			[[[window webView] mainFrame] loadRequest:[NSURLRequest requestWithURL:url]];
		}
	}

	bool OSXUserWindow::IsResizable()
	{
		return this->config->IsResizable();
	}

	void OSXUserWindow::SetResizable(bool resizable)
	{
		if (window != nil)
		{
			[window setShowsResizeIndicator:resizable];
			if (resizable)
			{
				[window setContentMinSize: NSMakeSize(config->GetMinWidth(), config->GetMinHeight())];
				[window setContentMaxSize: NSMakeSize(config->GetMaxWidth(), config->GetMaxHeight())];
			}
			else
			{
				[window setMinSize: [window frame].size];
				[window setMaxSize: [window frame].size];
			}
		}
	}

	bool OSXUserWindow::IsMaximizable()
	{
		return this->config->IsMaximizable();
	}

	void OSXUserWindow::SetMaximizable(bool maximizable)
	{
		if (window != nil)
		{
			[[window standardWindowButton:NSWindowZoomButton] setHidden:!maximizable];
		}
	}

	bool OSXUserWindow::IsMinimizable()
	{
		return this->config->IsMinimizable();
	}

	void OSXUserWindow::SetMinimizable(bool minimizable)
	{
		if (window != nil)
		{
			[[window standardWindowButton:NSWindowMiniaturizeButton] setHidden:!minimizable];
		}
	}

	bool OSXUserWindow::IsCloseable()
	{
		return this->config->IsCloseable();
	}

	void OSXUserWindow::SetCloseable(bool closeable)
	{
		if (window != nil)
		{
			[[window standardWindowButton:NSWindowCloseButton] setHidden:!closeable];
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
		if (window != nil)
		{
			[window setTransparency:transparency];
		}
	}

	void OSXUserWindow::SetFullScreen(bool fullscreen)
	{
		if (window != nil)
		{
			[window setFullScreen:fullscreen];
		}
	}

	void OSXUserWindow::SetUsingChrome(bool chrome)
	{
		this->config->SetUsingChrome(chrome);
	}

	void OSXUserWindow::SetMenu(SharedPtr<MenuItem> menu)
	{	
		if (menu == this->menu)
		{
			return;
		}
		this->menu = menu;
		if (focused)
		{
			SharedPtr<OSXMenuItem> m = menu.cast<OSXMenuItem>();
			this->osx_binding->WindowFocused(this,m.get());
		}
	}

	SharedPtr<MenuItem> OSXUserWindow::GetMenu()
	{
		return this->menu;
	}

	void OSXUserWindow::Focused()
	{
		this->focused = true;
		if (!menu.isNull())
		{
			SharedPtr<OSXMenuItem> m = menu.cast<OSXMenuItem>();
			this->osx_binding->WindowFocused(this,m.get());
		}
	}

	void OSXUserWindow::Unfocused()
	{
		this->focused = false;
		if (!menu.isNull())
		{
			SharedPtr<OSXMenuItem> m = menu.cast<OSXMenuItem>();
			this->osx_binding->WindowUnfocused(this,m.get());
		}
	}
	
	void OSXUserWindow::SetContextMenu(SharedPtr<MenuItem> value)
	{
		this->context_menu = value;
	}

	SharedPtr<MenuItem> OSXUserWindow::GetContextMenu()
	{
		return this->context_menu;
	}

	void OSXUserWindow::SetIcon(SharedString icon_path)
	{
		STUB();
	}

	SharedString OSXUserWindow::GetIcon()
	{
		STUB();
		return NULL;
	}

	bool OSXUserWindow::IsTopMost()
	{
		return this->topmost;
	}

	void OSXUserWindow::SetTopMost(bool topmost)
	{
		if (window != nil)
		{
			if (topmost)
			{
				[window setLevel:NSPopUpMenuWindowLevel];
				this->topmost = true;
			}
			else
			{
				[window setLevel:NSNormalWindowLevel];
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
			filename = [NSString stringWithCString:defaultName.c_str()];
		}
		if (!path.empty())
		{
			begin = [NSString stringWithCString:path.c_str()];
		}
		if (types.size() > 0)
		{
			filetypes = [[NSMutableArray alloc] init];
			for (size_t t = 0; t < types.size(); t++)
			{
				const char *s = types.at(t).c_str();
				[filetypes addObject:[NSString stringWithCString:s]];
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
			[filetypes addObject:[NSString stringWithCString:ft.c_str()]];
		}

		NSSavePanel *sp = [NSSavePanel savePanel];
		[sp setTitle:[NSString stringWithUTF8String:title.c_str()]];

		if ([filetypes count] > 0)
		{
			[sp setAllowedFileTypes:filetypes];
		}

		runResult = [sp runModalForDirectory:[NSString stringWithCString:path.c_str()] file:[NSString stringWithCString:defaultName.c_str()]];

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
    
