/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "ui_module.h"

using namespace ti;
UserWindow::UserWindow(WindowConfig *config, AutoUserWindow& parent) :
	KEventObject("UserWindow"),
	logger(Logger::Get("UI.UserWindow")),
	binding(UIModule::GetInstance()->GetUIBinding()),
	host(kroll::Host::GetInstance()),
	config(config),
	parent(parent),
	next_listener_id(0),
	active(false),
	initialized(false)
{
	this->shared_this = this;

	// This method is on Titanium.UI, but will be delegated to this class.
	/**
	 * @tiapi(method=True,name=UI.getCurrentWindow,since=0.4) Returns the current window
	 */
	this->SetMethod("getCurrentWindow", &UserWindow::_GetCurrentWindow);

	// @tiproperty[integer, UI.UserWindow.CENTERED,since=0.3,deprecated=true] The CENTERED event constant
	this->Set("CENTERED", Value::NewInt(UIBinding::CENTERED));

	/**
	 * @tiapi(method=True,name=UI.UserWindow.hide,since=0.2) Hides a window
	 */
	this->SetMethod("hide", &UserWindow::_Hide);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.show,since=0.2) Shows a window
	 */
	this->SetMethod("show", &UserWindow::_Show);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.minimize,since=0.4) Minimizes a window
	 */
	this->SetMethod("minimize", &UserWindow::_Minimize);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.unminimize,since=0.4) Unminimizes a window
	 */
	this->SetMethod("unminimize", &UserWindow::_Unminimize);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.maximize,since=0.4) Maximizes a window
	 */
	this->SetMethod("maximize", &UserWindow::_Maximize);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.unmaximize,since=0.4) Unmaximizes a window
	 */
	this->SetMethod("unmaximize", &UserWindow::_Unmaximize);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.focus,since=0.2) Focuses a window
	 */
	this->SetMethod("focus", &UserWindow::_Focus);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.unfocus,since=0.2) Unfocuses a window
	 */
	this->SetMethod("unfocus", &UserWindow::_Unfocus);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.isUsingChrome,since=0.2) Checks whether a window uses system chrome
	 * @tiresult(for=UI.UserWindow.isUsingChrome,type=boolean) true if the window uses system chrome, false if otherwise
	 */
	this->SetMethod("isUsingChrome", &UserWindow::_IsUsingChrome);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setUsingChrome,since=0.2) Sets whether a window should use system chrome
	 * @tiarg(for=UI.UserWindow.setUsingChrome,name=chrome,type=boolean) set to true to use system chrome, false if otherwise
	 */
	this->SetMethod("setUsingChrome", &UserWindow::_SetUsingChrome);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.isFullscreen,since=1.0) Checks whether a window is in fullscreen
	 * @tiarg(for=UI.UserWindow.isFullscreen,name=chrome,type=boolean) true if the window is in fullscreen, false if otherwise
	 */
	this->SetMethod("isFullscreen", &UserWindow::_IsFullscreen);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setFullscreen,since=1.0) Makes a window fullscreen
	 * @tiarg(for=UI.UserWindow.setFullscreen,name=fullscreen,type=boolean) set to true for fullscreen, false if otherwise
	 */
	this->SetMethod("setFullscreen", &UserWindow::_SetFullscreen);

	/**
	 * @tiapi(method=True,returns=integer,name=UI.UserWindow.getID,since=0.2) Returns the id of a window
	 * @tiresult(for=UI.UserWindow.getID,type=string) the id of the window
	 */
	this->SetMethod("getID", &UserWindow::_GetId);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.open,since=0.2) Opens a window
	 */
	this->SetMethod("open", &UserWindow::_Open);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.close,since=0.2) Closes a window
	 */
	this->SetMethod("close", &UserWindow::_Close);

	/**
	 * @tiapi(method=True,returns=double,name=UI.UserWindow.getX,since=0.2) Returns a window's horizontal (X-axis) position
	 * @tiresult(for=UI.UserWindow.getX,type=double) the horizontal position of the window
	 */
	this->SetMethod("getX", &UserWindow::_GetX);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setX,since=0.2) Sets a window's horizontal (X-axis) position
	 * @tiarg(for=UI.UserWindow.setX,type=double,name=x) the horizontal position
	 */
	this->SetMethod("setX", &UserWindow::_SetX);

	/**
	 * @tiapi(method=True,returns=double,name=UI.UserWindow.getY,since=0.2) Returns a window's vertical (Y-axis) position
	 * @tiresult(for=UI.UserWindow.getY,type=double) the vertical position of the window
	 */
	this->SetMethod("getY", &UserWindow::_GetY);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setY,since=0.2) Sets a window's vertical (Y-axis) position
	 * @tiarg(for=UI.UserWindow.setY,type=double,name=y) the vertical position
	 */
	this->SetMethod("setY", &UserWindow::_SetY);

	/**
	 * @tiapi(method=True,returns=double,name=UI.UserWindow.getWidth,since=0.2) Returns a window's width
	 * @tiresult(for=UI.UserWindow.getWidth,type=double) the width of the window
	 */
	this->SetMethod("getWidth", &UserWindow::_GetWidth);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setWidth,since=0.2) Sets a window's width
	 * @tiarg(for=UI.UserWindow.setWidth,type=double,name=width) the width of the window
	 */
	this->SetMethod("setWidth", &UserWindow::_SetWidth);

	/**
	 * @tiapi(method=True,returns=double,name=UI.UserWindow.getMaxWidth,since=0.2) Returns a window's max-width
	 * @tiresult(for=UI.UserWindow.getMaxWidth,type=double) the max-width value of the window
	 */
	this->SetMethod("getMaxWidth", &UserWindow::_GetMaxWidth);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setMaxWidth,since=0.2) Sets a window's max-width
	 * @tiarg(for=UI.UserWindow.setMaxWidth,type=double,name=width) the max-width value of the window
	 */
	this->SetMethod("setMaxWidth", &UserWindow::_SetMaxWidth);

	/**
	 * @tiapi(method=True,returns=double,name=UI.UserWindow.getMinWidth,since=0.2) Returns a window's min-width
	 * @tiresult(for=UI.UserWindow.getMinWidth,type=double) the min-width value of the window
	 */
	this->SetMethod("getMinWidth", &UserWindow::_GetMinWidth);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setMinWidth,since=0.2) Sets a window's min-width
	 * @tiarg(for=UI.UserWindow.setMinWidth,type=double,name=width) the min-width value of the window
	 */
	this->SetMethod("setMinWidth", &UserWindow::_SetMinWidth);

	/**
	 * @tiapi(method=True,returns=double,name=UI.UserWindow.getHeight,since=0.2) Returns a window's height
	 * @tiresult(for=UI.UserWindow.getHeight,type=double) the height value of the window
	 */
	this->SetMethod("getHeight", &UserWindow::_GetHeight);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setHeight,since=0.2) Sets a window's height
	 * @tiarg(for=UI.UserWindow.setHeight,type=double,name=height) the height value of the window
	 */
	this->SetMethod("setHeight", &UserWindow::_SetHeight);

	/**
	 * @tiapi(method=True,returns=double,name=UI.UserWindow.getMaxHeight,since=0.2) Returns a window's max height
	 * @tiresult(for=UI.UserWindow.getMaxHeight,type=double) the max-height value of the window
	 */
	this->SetMethod("getMaxHeight", &UserWindow::_GetMaxHeight);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setMaxHeight,since=0.2) Sets a window's max-height
	 * @tiarg(for=UI.UserWindow.setMaxHeight,type=double,name=height) the max-height value of the window
	 */
	this->SetMethod("setMaxHeight", &UserWindow::_SetMaxHeight);

	/**
	 * @tiapi(method=True,returns=double,name=UI.UserWindow.getMinHeight,since=0.2) Returns a window's min-height
	 * @tiresult(for=UI.UserWindow.getMinHeight,type=double) the min-height value of the window
	 */
	this->SetMethod("getMinHeight", &UserWindow::_GetMinHeight);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setMinHeight,since=0.2) Sets a window's min height
	 * @tiarg(for=UI.UserWindow.setMinHeight,type=double,name=height) the min-height value of the window
	 */
	this->SetMethod("setMinHeight", &UserWindow::_SetMinHeight);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.getBounds,since=0.2) Returns the window bounds
	 * @tiresult(for=UI.UserWindow.getBounds,type=object) an object containing the value for the window bounds
	 */
	this->SetMethod("getBounds", &UserWindow::_GetBounds);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setBounds,since=0.2) Sets the window bounds
	 * @tiarg(for=UI.UserWindow.setBounds,type=object,name=bounds) an object containing the value for the window bounds
	 */
	this->SetMethod("setBounds", &UserWindow::_SetBounds);

	/**
	 * @tiapi(method=True,returns=string,name=UI.UserWindow.getTitle,since=0.2) Returns the title of a window
	 * @tiresult(for=UI.UserWindow.isUsingChrome,type=boolean) the title of the window
	 */
	this->SetMethod("getTitle", &UserWindow::_GetTitle);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setTitle,since=0.2) Sets the title of a window
	 * @tiarg(for=UI.UserWindow.setTitle,type=object,name=title) the title of the window
	 */
	this->SetMethod("setTitle", &UserWindow::_SetTitle);

	/**
	 * @tiapi(method=True,returns=string,name=UI.UserWindow.getURL,since=0.2) Returns the url for a window
	 * @tiresult(for=UI.UserWindow.isUsingChrome,type=boolean) the url for the window
	 */
	this->SetMethod("getURL", &UserWindow::_GetURL);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setURL,since=0.2) Sets the url for a window
	 * @tiarg(for=UI.UserWindow.setURL,type=string,name=url) the url for the window
	 */
	this->SetMethod("setURL", &UserWindow::_SetURL);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.isResizable,since=0.2) Checks whether a window is resizable
	 * @tiresult(for=UI.UserWindow.isResizable,type=boolean) true if the window is resizable, false if otherwise
	 */
	this->SetMethod("isResizable", &UserWindow::_IsResizable);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setResizable,since=0.2) Sets whether a window could be resized or not
	 * @tiarg(for=UI.UserWindow.setResizable,type=boolean,name=resizable) true if the window could be resized, false if otherwise
	 */
	this->SetMethod("setResizable", &UserWindow::_SetResizable);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.isMaximized,since=0.4) Checks whether a window is maximized
	 * @tiresult(for=UI.UserWindow.isMaximized,type=boolean) true if the window is maximized, false if otherwise
	 */
	this->SetMethod("isMaximized", &UserWindow::_IsMaximized);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.isMinimized,since=0.4) Checks whether a window is minimized
	 * @tiresult(for=UI.UserWindow.isMinimized,type=boolean) true if the window is minimized, false if otherwise
	 */
	this->SetMethod("isMinimized", &UserWindow::_IsMinimized);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.isMaximizable,since=0.2) Checks whether a window could be maximized or not
	 * @tiresult(for=UI.UserWindow.isUsingChrome,type=boolean) true if the window could be maximized, false if otherwise
	 */
	this->SetMethod("isMaximizable", &UserWindow::_IsMaximizable);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setMaximizable,since=0.2) Sets whether a window could be maximized or not
	 * @tiarg(for=UI.UserWindow.setMaximizable,type=boolean,name=maximizable) true if the window could be maximized, false if otherwise
	 */
	this->SetMethod("setMaximizable", &UserWindow::_SetMaximizable);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.isMinimizable,since=0.2) Checks whether a window could be minimized or not
	 * @tiresult(for=UI.UserWindow.isMinimizable,type=boolean) true if the window could be minimized, false if otherwise
	 */
	this->SetMethod("isMinimizable", &UserWindow::_IsMinimizable);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setMinimizable,since=0.2) Sets whether a window could be maximized or not
	 * @tiarg(for=UI.UserWindow.setMinimizable,type=boolean,name=minimizable) true if the window could be minimized, false if otherwise
	 */
	this->SetMethod("setMinimizable", &UserWindow::_SetMinimizable);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.isCloseable,since=0.2) Checks whether a window could be closed or not
	 * @tiresult(for=UI.UserWindow.isCloseable,type=boolean) true if the window could be closed, false if otherwise
	 */
	this->SetMethod("isCloseable", &UserWindow::_IsCloseable);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setCloseable,since=0.2) Sets whether a window could be closed or not
	 * @tiarg(for=UI.UserWindow.setCloseable,type=boolean,name=closeable) true if the window could be closed, false if otherwise
	 */
	this->SetMethod("setCloseable", &UserWindow::_SetCloseable);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.isVisible,since=0.2) Checks whether a window is visible
	 * @tiresult(for=UI.UserWindow.isVisible,type=boolean) true if the window is visible, false if otherwise
	 */
	this->SetMethod("isVisible", &UserWindow::_IsVisible);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setVisible,since=0.2) Sets the visibility of the window
	 * @tiarg(for=UI.UserWindow.setVisible,type=boolean,name=visible) true if the window should be visible, false if otherwise
	 */
	this->SetMethod("setVisible", &UserWindow::_SetVisible);

	/**
	 * @tiapi(method=True,returns=double,name=UI.UserWindow.getTransparency,since=0.2) Returns a window's transparency value
	 * @tiresult(for=UI.UserWindow.getTransparency,type=double) the transparency value of the window
	 */
	this->SetMethod("getTransparency", &UserWindow::_GetTransparency);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setTransparency,since=0.2) Sets a window's transparency value
	 * @tiarg(for=UI.UserWindow.setTransparency,type=double,name=url) the transparency value of the window
	 */

	this->SetMethod("setTransparency", &UserWindow::_SetTransparency);

	/**
	 * @tiapi(method=True,returns=string,name=UI.UserWindow.getTransparencyColor,since=0.2) Returns a transparency color for the window
	 * @tiresult(for=UI.UserWindow.getTransparencyColor,type=string) the transparency color of the window
	 */
	this->SetMethod("getTransparencyColor", &UserWindow::_GetTransparencyColor);


	/**
	 * @tiapi(method=True,name=UI.UserWindow.setMenu,since=1.0)
	 * @tiapi Set this window's menu
	 * @tiarg[UI.Menu|null, menu] The Menu object to use as the menu or null to unset
	 */
	this->SetMethod("setMenu", &UserWindow::_SetMenu);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.getMenu,since=1.0) 
	 * Get this window's menu
	 * @tiresult[UI.Menu|null] This window's Menu or null if it is unset
	 */
	this->SetMethod("getMenu", &UserWindow::_GetMenu);


	/**
	 * @tiapi(method=True,name=UI.UserWindow.setContextMenu,since=1.0)
	 * @tiapi Set this window's context menu
	 * @tiarg[UI.Menu|null, menu] The Menu object to use as the context menu or null to unset
	 */
	this->SetMethod("setContextMenu", &UserWindow::_SetContextMenu);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.getContetMenu,since=1.0)
	 * Get this window's context menu
	 * @tiresult[UI.Menu|null] This window's context menu or null if it is unset
	 */
	this->SetMethod("getContextMenu", &UserWindow::_GetContextMenu);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setIcon,since=0.2) Sets a window's icon
	 * @tiarg(for=UI.UserWindow.setIcon,type=String,name=icon) path to the icon file
	 */
	this->SetMethod("setIcon", &UserWindow::_SetIcon);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.getIcon,since=0.2) Returns a window's icon
	 * @tiresult(for=UI.UserWindow.getIcon,type=string) path to the icon file
	 */
	this->SetMethod("getIcon", &UserWindow::_GetIcon);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.setTopMost,since=0.3) Sets whether a window is top most (above other windows)
	 * @tiarg(for=UI.UserWindow.setTopMost,type=boolean,name=topmost) true if top most, false if otherwise
	 */
	this->SetMethod("setTopMost", &UserWindow::_SetTopMost);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.isTopMost,since=0.3) Checks whether a window is top most
	 * @tiresult(for=UI.UserWindow.isTopMost,type=boolean) true if top most, false if otherwise
	 */
	this->SetMethod("isTopMost", &UserWindow::_IsTopMost);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.createWindow,since=0.2) Creates a new window as a child of the current window
	 * @tiarg(for=UI.UserWindow.createWindow,name=options,type=mixed,optional=True) a string containing a url of the new window's content or an object containing properties for the new window
	 * @tiresult(for=UI.UserWindow.createWindow,type=object) a UserWindow object
	 */
	this->SetMethod("createWindow", &UserWindow::_CreateWindow);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.openFileChooserDialog,since=0.4) Displays the file chooser dialog
	 * @tiarg(for=UI.UserWindow.openFileChooserDialog,type=method,name=callback) a callback function to fire after the user closes the dialog
	 * @tiarg(for=UI.UserWindow.openFileChooserDialog,type=object,name=options,optional=True) additional options for the dialog
	 */
	this->SetMethod("openFileChooserDialog", &UserWindow::_OpenFileChooserDialog);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.openFolderChooserDialog,since=0.4) Displays the folder chooser dialog
	 * @tiarg(for=UI.UserWindow.openFolderChooserDialog,type=method,name=callback) a callback function to fire after the user closes the dialog
	 * @tiarg(for=UI.UserWindow.openFolderChooserDialog,type=object,name=options,optional=True) additional options for the dialog
	 */
	this->SetMethod("openFolderChooserDialog", &UserWindow::_OpenFolderChooserDialog);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.openSaveAsDialog,since=0.4) Displays the save as file dialog
	 * @tiarg(for=UI.UserWindow.openSaveAsDialog,type=method,name=callback) a callback function to fire after the user closes the dialog
	 * @tiarg(for=UI.UserWindow.openSaveAsDialog,type=object,name=options,optional=True) additional options for the dialog
	 */
	this->SetMethod("openSaveAsDialog", &UserWindow::_OpenSaveAsDialog);

	/**
	 * @tiapi(method=True,name=UI.UserWindow.getParent,since=0.3) Returns the parent window
	 * @tiresult(for=UI.UserWindow.getParent,type=mixed) a UserWindow object referencing the parent window or null if no parent
	 */
	this->SetMethod("getParent", &UserWindow::_GetParent);

	this->FireEvent(Event::CREATED);
}

UserWindow::~UserWindow()
{
	if (this->active)
	{
		this->Close();
	}
}

AutoUserWindow UserWindow::GetAutoPtr()
{
	return this->shared_this;
}

Host* UserWindow::GetHost()
{
	return this->host;
}

void UserWindow::Open()
{
	this->FireEvent(Event::OPEN);

	// We are now in the UI binding's open window list
	this->binding->AddToOpenWindows(this->shared_this);

	// Tell the parent window that we are open for business
	if (!parent.isNull())
	{
		parent->AddChild(this->shared_this);
	}

	this->initialized = true;
	this->active = true;
}

void UserWindow::Close()
{
	// We want to fire the CLOSE event synchronously, because we
	// want to ensure that listeners on the originating window get
	// this event.
	this->FireEvent(Event::CLOSE);
	this->active = false; // prevent further modification
}

void UserWindow::Closed()
{
	this->FireEvent(Event::CLOSED);

	// Close all children and cleanup
	std::vector<AutoUserWindow>::iterator iter = this->children.begin();
	while (iter != this->children.end())
	{
		// Save a pointer to the child here, because it may
		// be freed by the SharedPtr otherwise and that will
		// make this iterator seriously, seriously unhappy.
		AutoUserWindow child = (*iter);
		iter = children.erase(iter);
		child->Close();
	}

	// Tell our parent that we are now closed
	if (!this->parent.isNull())
	{
		this->parent->RemoveChild(this->shared_this);
		this->parent->Focus(); // Focus the parent
	}

	// Tell the UIBinding that we are closed
	this->binding->RemoveFromOpenWindows(this->shared_this);

	// When we have no more open windows, we exit...
	std::vector<AutoUserWindow> windows = this->binding->GetOpenWindows();
	if (windows.size() == 0) {
		this->host->Exit(0);
	} else {
		windows.at(0)->Focus();
	}

	// This should be the last reference to this window
	// after all external references are destroyed.
	this->shared_this = NULL;
}

void UserWindow::_GetCurrentWindow(const kroll::ValueList& args, kroll::SharedValue result)
{
	result->SetObject(this->shared_this);
}

void UserWindow::_Hide(const kroll::ValueList& args, kroll::SharedValue result)
{
	this->config->SetVisible(false);
	if (this->active)
	{
		this->Hide();
	}
}

void UserWindow::_Show(const kroll::ValueList& args, kroll::SharedValue result)
{
	this->config->SetVisible(true);
	if (this->active)
	{
		this->Show();
	}
}

void UserWindow::_Minimize(const kroll::ValueList& args, kroll::SharedValue result)
{
	this->config->SetMinimized(true);
	if (this->active)
	{
		this->Minimize();
	}
}

void UserWindow::_Unminimize(const kroll::ValueList& args, kroll::SharedValue result)
{
	this->config->SetMinimized(false);
	if (this->active)
	{
		this->Unminimize();
	}
}

void UserWindow::_IsMinimized(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsMinimized());
	}
	else
	{
		return result->SetBool(this->config->IsMinimized());
	}
}

void UserWindow::_Maximize(const kroll::ValueList& args, kroll::SharedValue result)
{
	this->config->SetMaximized(true);
	if (this->active)
	{
		this->Maximize();
	}
}

void UserWindow::_IsMaximized(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsMaximized());
	}
	else
	{
		result->SetBool(this->config->IsMaximized());
	}
}

void UserWindow::_Unmaximize(const kroll::ValueList& args, kroll::SharedValue result)
{
	this->config->SetMaximized(false);
	if (this->active)
	{
		this->Unmaximize();
	}
}

void UserWindow::_Focus(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		this->Focus();
	}
}

void UserWindow::_Unfocus(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		this->Unfocus();
	}
}

void UserWindow::_IsUsingChrome(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsUsingChrome());
	}
	else
	{
		result->SetBool(this->config->IsUsingChrome());
	}
}

void UserWindow::_SetTopMost(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setTopMost", "b");
	bool b = args.at(0)->ToBool();
	this->config->SetTopMost(b);
	if (this->active)
	{
		this->SetTopMost(b);
	}
}

void UserWindow::_IsTopMost(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsTopMost());
	}
	else
	{
		result->SetBool(this->config->IsTopMost());
	}
}

void UserWindow::_SetUsingChrome(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setUsingChrome", "b");
	bool b = args.at(0)->ToBool();
	this->config->SetUsingChrome(b);
	if (this->active)
	{
		this->SetUsingChrome(b);
	}
}

void UserWindow::_IsUsingScrollbars(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsUsingChrome());
	}
	else
	{
		result->SetBool(this->config->IsUsingScrollbars());
	}
}

void UserWindow::_IsFullscreen(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsFullscreen());
	}
	else
	{
		result->SetBool(this->config->IsFullscreen());
	}
}

void UserWindow::_SetFullscreen(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setFullscreen", "b");
	bool b = args.at(0)->ToBool();
	this->config->SetFullscreen(b);
	if (this->active)
	{
		this->SetFullscreen(b);
	}
}

void UserWindow::_GetId(const kroll::ValueList& args, kroll::SharedValue result)
{
	result->SetString(this->config->GetID());
}

void UserWindow::_Open(const kroll::ValueList& args, kroll::SharedValue result)
{
	// Don't allow a window to be opened twice
	if (this->active || this->initialized)
	{
		throw ValueException::FromString("Cannot open a window more than once");
	}
	else
	{
		this->Open();
	}
}

void UserWindow::_Close(const kroll::ValueList& args, kroll::SharedValue result)
{
	// Don't allow a non-active window to be closed
	if (this->active)
	{
		this->Close();
	}
}

void UserWindow::_GetX(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetDouble(this->GetX());
	}
	else
	{
		result->SetDouble(this->config->GetX());
	}
}

void UserWindow::_SetX(const kroll::ValueList& args, kroll::SharedValue result)
{
	// Manual argument type-checking for speed considerations
	if (args.size() > 0 && args.at(0)->IsNumber())
	{
		double x = args.at(0)->ToNumber();
		this->config->SetX(x);
		if (this->active)
		{
			this->SetX(x);
		}
	}
}

void UserWindow::_GetY(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetDouble(this->GetY());
	}
	else
	{
		result->SetDouble(this->config->GetY());
	}
}

void UserWindow::_SetY(const kroll::ValueList& args, kroll::SharedValue result)
{
	// Manual argument type-checking for speed considerations
	if (args.size() > 0 && args.at(0)->IsNumber())
	{
		double y = args.at(0)->ToNumber();
		this->config->SetY(y);
		if (this->active)
		{
			this->SetY(y);
		}
	}
}

void UserWindow::_GetWidth(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetDouble(this->GetWidth());
	}
	else
	{
		result->SetDouble(this->config->GetWidth());
	}
}

void UserWindow::_SetWidth(const kroll::ValueList& args, kroll::SharedValue result)
{
	// Manual argument type-checking for speed considerations
	if (args.size() > 0 && args.at(0)->IsNumber())
	{
		double w = args.at(0)->ToNumber();
		if (w > 0)
		{
			w = UserWindow::Constrain(w, config->GetMinWidth(), config->GetMaxWidth());
			this->config->SetWidth(w);
			if (this->active)
			{
				this->SetWidth(w);
			}
		}
	}
}

void UserWindow::_GetMinWidth(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetDouble(this->GetMinWidth());
	}
	else
	{
		result->SetDouble(this->config->GetMinWidth());
	}
}

void UserWindow::_SetMinWidth(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setMinWidth", "n");
	double mw = args.at(0)->ToNumber();

	if (mw <= 0)
	{
		mw = -1;
	}

	this->config->SetMinWidth(mw);
	if (mw != -1 && this->config->GetWidth() < mw)
	{
		this->config->SetWidth(mw);
	}

	if (this->active)
	{
		this->SetMinWidth(mw);
		if (mw != -1 && this->GetWidth() < mw)
		{
			this->SetWidth(mw);
		}
	}
}

void UserWindow::_GetMaxWidth(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetDouble(this->GetMaxWidth());
	}
	else
	{
		result->SetDouble(this->config->GetMaxWidth());
	}
}

void UserWindow::_SetMaxWidth(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setMaxWidth", "n");
	double mw = args.at(0)->ToNumber();

	if (mw <= 0)
	{
		mw = -1;
	}

	this->config->SetMaxWidth(mw);
	if (mw != -1 && this->config->GetWidth() > mw)
	{
		this->config->SetWidth(mw);
	}

	if (this->active)
	{
		this->SetMaxWidth(mw);
		if (mw != -1 && this->GetWidth() > mw)
		{
			this->SetWidth(mw);
		}
	}
}

void UserWindow::_GetHeight(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetDouble(this->GetHeight());
	}
	else
	{
		result->SetDouble(this->config->GetHeight());
	}
}

void UserWindow::_SetHeight(const kroll::ValueList& args, kroll::SharedValue result)
{
	// Manual argument type-checking for speed considerations
	if (args.size() > 0 && args.at(0)->IsNumber())
	{
		double h = args.at(0)->ToNumber();
		if (h > 0)
		{
			h = UserWindow::Constrain(h, config->GetMinHeight(), config->GetMaxHeight());
			this->config->SetHeight(h);
			if (this->active)
			{
				this->SetHeight(h);
			}
		}
	}
}

void UserWindow::_GetMinHeight(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetDouble(this->GetMinHeight());
	}
	else
	{
		result->SetDouble(this->config->GetMinHeight());
	}
}

void UserWindow::_SetMinHeight(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setMinHeight", "n");
	double mh = args.at(0)->ToNumber();
	if (mh <= 0)
	{
		mh = -1;
	}

	this->config->SetMinHeight(mh);
	if (mh != -1 && this->config->GetHeight() < mh)
	{
		this->config->SetHeight(mh);
	}

	if (this->active)
	{
		this->SetMinHeight(mh);
		if (mh != -1 && this->GetHeight() < mh)
		{
			this->SetHeight(mh);
		}
	}
}

void UserWindow::_GetMaxHeight(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetDouble(this->GetMaxHeight());
	}
	else
	{
		result->SetDouble(this->config->GetMaxHeight());
	}
}

void UserWindow::_SetMaxHeight(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setMaxHeight", "n");
	double mh = args.at(0)->ToNumber();
	if (mh <= 0)
	{
		mh = -1;
	}

	this->config->SetMaxHeight(mh);
	if (mh != -1 && this->config->GetHeight() > mh)
	{
		this->config->SetHeight(mh);
	}

	if (this->active)
	{
		this->SetMaxHeight(mh);
		if (mh != -1 && this->GetHeight() > mh)
		{
			this->SetHeight(mh);
		}
	}
}

void UserWindow::_GetBounds(const kroll::ValueList& args, kroll::SharedValue result)
{
	Bounds bounds;
	if (this->active)
	{
		bounds = this->GetBounds();
	}
	else
	{
		bounds.x = this->config->GetX();
		bounds.y = this->config->GetY();
		bounds.width = this->config->GetWidth();
		bounds.height = this->config->GetHeight();
	}

	kroll::StaticBoundObject *b = new kroll::StaticBoundObject();
	b->Set("x", kroll::Value::NewInt(bounds.x));
	b->Set("y", kroll::Value::NewInt(bounds.y));
	b->Set("width", kroll::Value::NewInt(bounds.width));
	b->Set("height", kroll::Value::NewInt(bounds.height));
	result->SetObject(b);
}

void UserWindow::_SetBounds(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (args.size() < 1 || !args.at(0)->IsObject())
	{
		throw ValueException::FromString("setBounds takes an object with x, y, width and height properties.");
		return;
	}

	SharedKObject o = args.at(0)->ToObject();
	if (!o->Get("x")->IsNumber()
		|| !o->Get("y")->IsNumber()
		|| !o->Get("width")->IsNumber()
		||!o->Get("height")->IsNumber())
	{
		throw ValueException::FromString("setBounds takes an object with x, y, width and height properties.");
		return;
	}

	double x = o->Get("x")->ToNumber();
	double y = o->Get("y")->ToNumber();
	double w = o->Get("width")->ToNumber();
	double h = o->Get("height")->ToNumber();
	w = UserWindow::Constrain(w, config->GetMinWidth(), config->GetMaxWidth());
	h = UserWindow::Constrain(h, config->GetMinHeight(), config->GetMaxHeight());

	this->config->SetX(x);
	this->config->SetY(y);
	this->config->SetWidth(w);
	this->config->SetHeight(h);

	if (this->active)
	{
		Bounds bounds;
		bounds.x = x;
		bounds.y = y;
		bounds.width = w;
		bounds.height = h;
		this->SetBounds(bounds);
	}
}

void UserWindow::_GetTitle(const kroll::ValueList& args, kroll::SharedValue result)
{
	std::string title;
	if (this->active)
	{
		title = this->GetTitle();
	}
	else
	{
		title = this->config->GetTitle();
	}
	result->SetString(title);
}

void UserWindow::_SetTitle(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setTitle", "s");

	std::string title = args.at(0)->ToString();
	this->config->SetTitle(title);
	if (this->active)
	{
		this->SetTitle(title);
	}
}

void UserWindow::_GetURL(const kroll::ValueList& args, kroll::SharedValue result)
{
	string url;
	if (this->active)
	{
		url = this->GetURL();
	}
	else
	{
		url = this->config->GetURL();
	}
	result->SetString(url);
}

void UserWindow::_SetURL(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setURL", "s");

	std::string url = args.at(0)->ToString();
	url = AppConfig::Instance()->InsertAppIDIntoURL(url);
	this->config->SetURL(url);
	if (this->active)
	{
		this->SetURL(url);
	}
}

void UserWindow::_IsResizable(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsResizable());
	}
	else
	{
		result->SetBool(this->config->IsResizable());
	}
}

void UserWindow::_SetResizable(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setResizable", "b");
	bool b = args.at(0)->ToBool();
	this->config->SetResizable(b);
	if (this->active)
	{
		this->SetResizable(b);
	}
}

void UserWindow::_IsMaximizable(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsMaximizable());
	}
	else
	{
		result->SetBool(this->config->IsMaximizable());
	}
}

void UserWindow::_SetMaximizable(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setMaximizable", "b");
	bool b = args.at(0)->ToBool();
	this->config->SetMaximizable(b);
	if (this->active)
	{
		this->SetMaximizable(b);
	}
}

void UserWindow::_IsMinimizable(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsMinimizable());
	}
	else
	{
		result->SetBool(this->config->IsMinimizable());
	}
}

void UserWindow::_SetMinimizable(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setMinimizable", "b");
	bool b = args.at(0)->ToBool();
	this->config->SetMinimizable(b);
	if (this->active)
	{
		this->SetMinimizable(b);
	}
}

void UserWindow::_IsCloseable(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsCloseable());
	}
	else
	{
		result->SetBool(this->config->IsCloseable());
	}
}

void UserWindow::_SetCloseable(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setCloseable", "b");
	bool b = args.at(0)->ToBool();
	this->config->SetCloseable(b);
	if (this->active)
	{
		this->SetCloseable(b);
	}
}

void UserWindow::_IsVisible(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetBool(this->IsVisible());
	}
	else
	{
		result->SetBool(false);
	}
}

void UserWindow::_SetVisible(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setVisible", "b");
	bool b = args.at(0)->ToBool();
	this->config->SetVisible(b);

	if (this->active)
	{
		if (b)
		{
			this->Show();
		}
		else
		{
			this->Hide();
		}
	}
}

void UserWindow::_GetTransparency(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->active)
	{
		result->SetDouble(this->GetTransparency());
	}
	else
	{
		result->SetDouble(this->config->GetTransparency());
	}
}

void UserWindow::_SetTransparency(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setTransparency", "n");
	double t = args.at(0)->ToNumber();
	t = UserWindow::Constrain(t, 0.0, 1.0);

	this->config->SetTransparency(t);
	if (this->active)
	{
		this->SetTransparency(t);
	}
}

void UserWindow::_GetTransparencyColor(const kroll::ValueList& args, kroll::SharedValue result)
{
	std::string color = this->GetTransparencyColor();
	result->SetString(color);
}

void UserWindow::_SetMenu(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setMenu", "?o");
	AutoMenu menu = NULL;
	if (args.size() > 0) {
		menu = args.at(0)->ToObject().cast<Menu>();
	}
	this->SetMenu(menu);
}

void UserWindow::_GetMenu(const kroll::ValueList& args, kroll::SharedValue result)
{
	AutoMenu menu = this->GetMenu();
	if (!menu.isNull())
	{
		result->SetObject(menu);
	}
	else
	{
		result->SetNull();
	}
}

void UserWindow::_SetContextMenu(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setContextMenu", "?o");
	AutoMenu menu = NULL;
	if (args.size() > 0) {
		menu = args.at(0)->ToObject().cast<Menu>();
	}
	this->SetContextMenu(menu);
}

void UserWindow::_GetContextMenu(const kroll::ValueList& args, kroll::SharedValue result)
{
	AutoMenu menu = this->GetContextMenu();
	if (!menu.isNull())
	{
		result->SetObject(menu);
	}
	else
	{
		result->SetNull();
	}
}

void UserWindow::_SetIcon(const kroll::ValueList& args, kroll::SharedValue result)
{
	args.VerifyException("setIcon", "s|0");
	std::string iconPath;
	if (args.size() > 0) {
		std::string in = args.GetString(0);
		iconPath = URLToPathOrURL(in);
	}

	this->SetIcon(iconPath);
}

void UserWindow::_GetIcon(const kroll::ValueList& args, kroll::SharedValue result)
{
	result->SetString(this->GetIcon());
}

void UserWindow::_GetParent(const kroll::ValueList& args, kroll::SharedValue result)
{
	if (this->parent.isNull())
	{
		result->SetNull();
	}
	else
	{
		result->SetObject(this->parent);
	}
}

void UserWindow::_CreateWindow(const ValueList& args, SharedValue result)
{
	//TODO: wrap in sharedptr
	WindowConfig *config = NULL;

	if (args.size() > 0 && args.at(0)->IsObject())
	{
		SharedKObject props = SharedKObject(new StaticBoundObject());
		config = new WindowConfig();
		props = args.at(0)->ToObject();
		config->UseProperties(props);
	}
	else if (args.size() > 0 && args.at(0)->IsString())
	{
		// String might match a url spec
		std::string url = args.at(0)->ToString();
		WindowConfig* matchedConfig = AppConfig::Instance()->GetWindowByURL(url);

		url = AppConfig::Instance()->InsertAppIDIntoURL(url);
		config = new WindowConfig(matchedConfig, url);
	}
	else
	{
		config = new WindowConfig();
	}

	AutoUserWindow new_window = this->binding->CreateWindow(config, shared_this);
	result->SetObject(new_window);
}

void UserWindow::UpdateWindowForURL(std::string url)
{
	WindowConfig* config = AppConfig::Instance()->GetWindowByURL(url);
	if (!config)
	{
		// no need to update window
		return;
	}

	// copy the config object
	config = new WindowConfig(config, url);

	Bounds b;
	b.x = config->GetX();
	b.y = config->GetY();
	b.width = config->GetWidth();
	b.height = config->GetHeight();

	this->SetBounds(b);

	this->SetMinimizable(config->IsMinimizable());
	this->SetMaximizable(config->IsMaximizable());
	this->SetCloseable(config->IsCloseable());
}

void UserWindow::ReadChooserDialogObject(
	SharedKObject o,
	bool& multiple,
	std::string& title,
	std::string& path,
	std::string& defaultName,
	std::vector<std::string>& types,
	std::string& typesDescription)

{
	// Pass in a set of properties for chooser dialogs like this:
	// var selected = Titanium.UI.OpenFileChooserDialog(callback,
	// {
	//    multiple:true,
	//    title: "Select file to delete...",
	//    defaultFile: "autoexec.bat",
	//    path: "C:\"
	//    types:['js','html']
	// });
	multiple = o->GetBool("multiple", true);
	title = o->GetString("title", title);
	path = o->GetString("path", path);
	defaultName = o->GetString("defaultName", defaultName);

	SharedKList listTypes = new StaticBoundList();
	listTypes = o->GetList("types", listTypes);
	for (size_t i = 0; i < listTypes->Size(); i++)
	{
		if (listTypes->At(i)->IsString())
		{
			types.push_back(listTypes->At(i)->ToString());
			std::cout << "Found " << listTypes->At(i)->ToString() << std::endl;
		}
	}
	typesDescription = o->GetString("typesDescription", defaultName);

}

void UserWindow::_OpenFileChooserDialog(const ValueList& args, SharedValue result)
{
	args.VerifyException("openFileChooserDialog", "m,o?");

	SharedKMethod callback = args.at(0)->ToMethod();
	bool multiple = false;
	std::string path;
	std::string defaultName;
	std::string title = "Choose File";
	std::vector<std::string> types;
	std::string typesDescription;

	SharedKObject props;
	if (args.size() > 1)
	{
		SharedKObject props = args.at(1)->ToObject();
		ReadChooserDialogObject(props,
			multiple,
			title,
			path,
			defaultName,
			types,
			typesDescription);
	}
	this->OpenFileChooserDialog(
		callback, multiple, title, path, defaultName, types, typesDescription);
}

void UserWindow::_OpenFolderChooserDialog(const ValueList& args, SharedValue result)
{
	args.VerifyException("openFolderChooserDialog", "m,o?");
	SharedKMethod callback = args.at(0)->ToMethod();
	bool multiple = false;
	std::string path;
	std::string defaultName;
	std::string title = "Choose Directory";
	std::vector<std::string> types;
	std::string typesDescription;

	SharedKObject props;
	if (args.size() > 1)
	{
		SharedKObject props = args.at(1)->ToObject();
		ReadChooserDialogObject(props,
			multiple,
			title,
			path,
			defaultName,
			types,
			typesDescription);
	}
	this->OpenFolderChooserDialog(
		callback, multiple, title, path, defaultName);
}

void UserWindow::_OpenSaveAsDialog(const ValueList& args, SharedValue result)
{
	args.VerifyException("openFolderChooserDialog", "m,o?");
	SharedKMethod callback = args.at(0)->ToMethod();
	bool multiple = false;
	std::string path;
	std::string defaultName;
	std::string title = "Save File";
	std::vector<std::string> types;
	std::string typesDescription;

	SharedKObject props;
	if (args.size() > 1)
	{
		SharedKObject props = args.at(1)->ToObject();
		ReadChooserDialogObject(props,
			multiple,
			title,
			path,
			defaultName,
			types,
			typesDescription);
	}
	this->OpenSaveAsDialog(
		callback, title, path, defaultName, types, typesDescription);
}

AutoUserWindow UserWindow::GetParent()
{
	return this->parent;
}

void UserWindow::AddChild(AutoUserWindow child)
{
	this->children.push_back(child);
}

void UserWindow::RemoveChild(AutoUserWindow child)
{
	std::vector<AutoUserWindow>::iterator iter = this->children.begin();
	while (iter != this->children.end())
	{
		if ((*iter).get() == child.get())
		{
			iter = children.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

bool UserWindow::ShouldHaveTitaniumObject(
	JSGlobalContextRef ctx, JSObjectRef global)
{
	// We really only want URLs that are loaded via the
	// app, ti or file protocol to have the Titanium object.
	// Other URLs won't have access for security considerations.
	JSStringRef docPropName = JSStringCreateWithUTF8CString("document");
	JSValueRef docValue = JSObjectGetProperty(ctx, global, docPropName, NULL);
	if (!docValue)
	{
		return false;
	}

	JSObjectRef docObject = JSValueToObject(ctx, docValue, NULL);
	if (!docObject)
	{
		return false;
	}

	JSStringRef locPropName = JSStringCreateWithUTF8CString("location");
	JSValueRef locValue = JSObjectGetProperty(ctx, docObject, locPropName, NULL);
	if (!locValue)
	{
		return false;
	}

	JSStringRef locString = JSValueToStringCopy(ctx, locValue, NULL);
	if (!locString)
	{
		return false;
	}

	char* cStringURL = KJSUtil::ToChars(locString);
	string url = cStringURL;
	free(cStringURL);

	transform(url.begin(), url.end(), url.begin(), tolower);
	return url.find("app://") == 0 || 
		url.find("ti://") == 0 ||
		url.find("file://") == 0;
}

void UserWindow::RegisterJSContext(JSGlobalContextRef context)
{
	JSObjectRef globalObject = JSContextGetGlobalObject(context);
	KJSUtil::RegisterGlobalContext(globalObject, context);
	KJSUtil::ProtectGlobalContext(context);

	if (!this->ShouldHaveTitaniumObject(context, globalObject))
	{
		return;
	}

	// Produce a delegating object to represent the top-level
	// Titanium object. When a property isn't found in this object
	// it will look for it in global_tibo.
	SharedKObject global_tibo = this->host->GetGlobalObject();
	SharedKObject tiObject = new DelegateStaticBoundObject(global_tibo);

	SharedValue ui_api_value = tiObject->Get("UI");
	if (ui_api_value->IsObject())
	{
		// Create a delegate object for the UI API.
		SharedKObject ui_api = ui_api_value->ToObject();
		KObject* delegateUIAPI = new DelegateStaticBoundObject(ui_api, new AccessorBoundObject());

		// Place currentWindow in the delegate.
		delegateUIAPI->Set("getCurrentWindow", this->Get("getCurrentWindow"));

		// Place currentWindow.createWindow in the delegate.
		delegateUIAPI->Set("createWindow", this->Get("createWindow"));

		// Place currentWindow.openFiles in the delegate.
		delegateUIAPI->Set("openFileChooserDialog", this->Get("openFileChooserDialog"));
		delegateUIAPI->Set("openFolderChooserDialog", this->Get("openFolderChooserDialog"));
		delegateUIAPI->Set("openSaveAsDialog", this->Get("openSaveAsDialog"));

		tiObject->Set("UI", Value::NewObject(delegateUIAPI));
	}
	else
	{
		std::cerr << "Could not find UI API point!" << std::endl;
	}

	// Get the global object into a KKJSObject
	SharedKObject frameGlobal = new KKJSObject(context, globalObject);

	// Copy the document and window properties to the Titanium object
	SharedValue doc_value = frameGlobal->Get("document");
	tiObject->Set("document", doc_value);
	SharedValue windowValue = frameGlobal->Get("window");
	tiObject->Set("window", windowValue);

	// Place the Titanium object into the window's global object
	frameGlobal->SetObject(GLOBAL_NS_VARNAME, tiObject);

	// bind the window into currentWindow so you can call things like
	// Titanium.UI.currentWindow.getParent().window to get the parent's
	// window and global variable scope
	this->Set("window", windowValue);

	UserWindow::LoadUIJavaScript(context);

	AutoPtr<Event> event = new Event(this->GetAutoPtr(), Event::PAGE_INITIALIZED);
	event->SetObject("scope", frameGlobal);
	event->SetString("url", config->GetURL());
	this->FireEvent(event);
}

void UserWindow::LoadUIJavaScript(JSGlobalContextRef context)
{
	std::string modulePath = UIModule::GetInstance()->GetPath();
	std::string jsPath = FileUtils::Join(modulePath.c_str(), "ui.js", NULL);
	try
	{
		KJSUtil::EvaluateFile(context, (char*) jsPath.c_str());
	}
	catch (kroll::ValueException &e)
	{
		SharedString ss = e.DisplayString();
		Logger* logger = Logger::Get("UIModule");
		logger->Error("Error loading %s: %s", jsPath.c_str(), (*ss).c_str());
	}
	catch (...)
	{
		Logger* logger = Logger::Get("UIModule");
		logger->Error("Unexpected error loading %s", jsPath.c_str());
	}
}

void UserWindow::PageLoaded(
	SharedKObject globalObject, std::string &url, JSGlobalContextRef context)
{
	AutoPtr<Event> event = new Event(this->GetAutoPtr(), Event::PAGE_LOADED);
	event->SetObject("scope", globalObject);
	event->SetString("url", url);
	this->FireEvent(event);
}

double UserWindow::Constrain(double value, double min, double max)
{
	if (min >= 0.0 && value < min)
	{
		value = min;
	}
	if (max >= 0.0 && value > max)
	{
		value = max;
	}
	return value;
}
