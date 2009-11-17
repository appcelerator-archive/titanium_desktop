/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * SEE LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"
#include <sstream>
#include <cmath>

#define SetFlag(x,flag,b) ((b) ? x |= flag : x &= ~flag)
#define UnsetFlag(x,flag) (x &= ~flag)=
#define USERWINDOW_WINDOW_CLASS L"Win32UserWindow"

using namespace ti;

// slightly off white, there's probably a better way to do this
COLORREF transparencyColor = RGB(0xF9, 0xF9, 0xF9);

static void* SetWindowUserData(HWND hwnd, void* userData)
{
	return reinterpret_cast<void*>(SetWindowLongPtr(hwnd, GWLP_USERDATA,
		reinterpret_cast<LONG_PTR> (userData)));
}

static void* GetWindowUserData(HWND hWnd)
{
	return reinterpret_cast<void*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

static void HandleHResultError(std::string message, HRESULT result, bool fatal=false)
{
	static Logger* logger = Logger::Get("UI.Win32UserWindow");

	message.append(": ");
	switch (result)
	{
		case REGDB_E_CLASSNOTREG:
			message.append("REGDB_E_CLASSNOTREG");
			break;
		case CLASS_E_NOAGGREGATION:
			message.append("CLASS_E_NOAGGREGATION");
			break;
		case E_NOINTERFACE:
			message.append("E_NOINTERFACE");
			break;
		case E_UNEXPECTED:
			message.append("E_UNEXPECTED");
			break;
		case E_OUTOFMEMORY:
			message.append("E_OUTOFMEMORY");
			break;
		case E_INVALIDARG:
			message.append("E_INVALIDARG");
			break;
		default:
			message.append("Unknown Error (");
			message.append(KList::IntToChars(result));
			message.append(")");
			break;
	}

	logger->Error(message);
	Win32UIBinding::ErrorDialog(message);
	if (fatal)
		exit(1);
	else
		throw ValueException::FromString(message);
}

/*static*/
Win32UserWindow* Win32UserWindow::FromWindow(HWND hWnd)
{
	return reinterpret_cast<Win32UserWindow*> (GetWindowUserData(hWnd));
}

/*static*/
void Win32UserWindow::RegisterWindowClass(HINSTANCE hInstance)
{
	static bool classInitialized = false;
	if (!classInitialized)
	{
		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Win32UserWindow::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = 0;
		wcex.hIconSm = 0;
		wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
		wcex.hbrBackground = CreateSolidBrush(transparencyColor);
		wcex.lpszMenuName = L"";
		wcex.lpszClassName = USERWINDOW_WINDOW_CLASS;

		ATOM result = RegisterClassExW(&wcex);
		if (result == NULL)
		{
			Logger::Get("UI.Win32UserWindow")->Error("Error Registering Window Class: %d", GetLastError());
		}

		classInitialized = true;
	}
}

/*static*/
LRESULT CALLBACK
Win32UserWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Win32UserWindow* window = Win32UserWindow::FromWindow(hWnd);

	switch (message)
	{
		case WM_DESTROY:
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_CLOSE:
			if (window->Close())
				return DefWindowProc(hWnd, message, wParam, lParam);
			else
				return 0;

		case WM_GETMINMAXINFO:
			if (window)
			{
				window->GetMinMaxInfo((MINMAXINFO*) lParam);
			}
			break;

		case WM_SIZE:
			if (window->webView)
			{
				window->ResizeSubViews();
				window->FireEvent(Event::RESIZED);
				if (wParam == SIZE_MAXIMIZED)
				{
					window->FireEvent(Event::MAXIMIZED);
				}
				else if (wParam == SIZE_MINIMIZED)
				{
					window->FireEvent(Event::MINIMIZED);
				}
			}
			break;

		case WM_SETFOCUS:
			// The focus event will be fired by the UIDelegate
			window->Focus(); // Focus the WebView and not the main window.
			return 0;

		case WM_KILLFOCUS:
			// The unfocus event will be fired by the UIDelegate
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_MOVE:
			window->FireEvent(Event::MOVED);
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_SHOWWINDOW:
			window->FireEvent(((BOOL)wParam) ? Event::SHOWN : Event::HIDDEN);
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_MENUCOMMAND:
		{
			HMENU nativeMenu = (HMENU) lParam;
			UINT position = (UINT) wParam;
			UINT itemId = GetMenuItemID(nativeMenu, position);

			if (itemId == WEB_INSPECTOR_MENU_ITEM_ID)
			{
				Win32UserWindow* inspectorWindow = Win32UserWindow::FromWindow(hWnd);
				if (inspectorWindow)
					inspectorWindow->ShowInspector(false);
				break;

			}
			else if (Win32MenuItem::HandleClickEvent(nativeMenu, position))
			{
				break;
			}
			else
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

DWORD Win32UserWindow::GetStyleFromConfig() const
{
	DWORD style = WS_EX_APPWINDOW;
	if (config->IsToolWindow())
		style = WS_EX_TOOLWINDOW;

	if (config->GetTransparency() < 1.0)
		style |= WS_EX_LAYERED;

	return style;
}

void Win32UserWindow::InitWindow()
{
	Win32UserWindow::RegisterWindowClass(win32Host->GetInstanceHandle());

	std::wstring titleW = ::UTF8ToWide(config->GetTitle());
	this->windowHandle = CreateWindowExW(GetStyleFromConfig(), USERWINDOW_WINDOW_CLASS,
		titleW.c_str(), WS_CLIPCHILDREN, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL, NULL, win32Host->GetInstanceHandle(), NULL);

	if (this->windowHandle == NULL)
	{
		std::ostringstream error;
		error << "Error Creating Window: " << GetLastError();
		logger->Error(error.str());
	}

	// these APIs are semi-private -- we probably shouldn't mark them
	// make our HWND available to 3rd party devs without needing our headers
	KValueRef windowHandle = Value::NewVoidPtr((void*) this->windowHandle);
	this->Set("windowHandle", windowHandle);
	logger->Debug("Initializing windowHandle: %i", windowHandle);

	SetWindowUserData(this->windowHandle, this);
}

void Win32UserWindow::InitWebKit()
{
	HRESULT hr = WebKitCreateInstance(CLSID_WebView, 0, IID_IWebView,
		(void**) &(this->webView));
	if (FAILED(hr))
		HandleHResultError("Error creating WebKitWebView", hr, true);

	// TI-303 we need to add safari UA to our UA to resolve broken
	// sites that look at Safari and not WebKit for UA.
	_bstr_t ua("Version/4.0 Safari/528.16 "PRODUCT_NAME"/"PRODUCT_VERSION);
	webView->setApplicationNameForUserAgent(ua.copy());

	// place our user agent string in the global so we can later use it
	KObjectRef global = host->GetGlobalObject();
	if (global->Get("userAgent")->IsUndefined())
	{
		_bstr_t uaURL("http://titaniumapp.com");
		BSTR uaResp;
		webView->userAgentForURL(uaURL.copy(), &uaResp);
		std::string uaStr = _bstr_t(uaResp);
		global->Set("userAgent", Value::NewString(uaStr.c_str()));
	}

	frameLoadDelegate = new Win32WebKitFrameLoadDelegate(this);
	hr = webView->setFrameLoadDelegate(frameLoadDelegate);
	if (FAILED(hr))
		HandleHResultError("Error setting FrameLoadDelegate", hr, true);

	uiDelegate = new Win32WebKitUIDelegate(this);
	hr = webView->setUIDelegate(uiDelegate);
	if (FAILED(hr))
		HandleHResultError("Error setting UIDelegate", hr, true);

	policyDelegate = new Win32WebKitPolicyDelegate(this);
	hr = webView->setPolicyDelegate(policyDelegate);
	if (FAILED(hr))
		HandleHResultError("Error setting PolicyDelegate", hr, true);

	resourceLoadDelegate = new Win32WebKitResourceLoadDelegate(this);
	hr = webView->setResourceLoadDelegate(resourceLoadDelegate);
	if (FAILED(hr))
		HandleHResultError("Error setting ResourceLoadDelegate", hr, true);

	hr = webView->setHostWindow((OLE_HANDLE) windowHandle);
	if (FAILED(hr))
		HandleHResultError("Error setting host window", hr, true);

	RECT clientRect;
	GetClientRect(windowHandle, &clientRect);
	hr = webView->initWithFrame(clientRect, 0, 0);
	if (FAILED(hr))
		HandleHResultError("Could not intialize WebView with frame", hr, true);

	IWebPreferences *prefs = NULL;
	hr = WebKitCreateInstance(CLSID_WebPreferences, 0, IID_IWebPreferences,
		(void**) &prefs);
	if (FAILED(hr) || !prefs)
		HandleHResultError("Error getting IWebPreferences", hr, true);

	std::string appid(AppConfig::Instance()->GetAppID());
	_bstr_t pi(appid.c_str());
	prefs->initWithIdentifier(pi.copy(), &prefs);
	prefs->setCacheModel(WebCacheModelDocumentBrowser);
	prefs->setPlugInsEnabled(true);
	prefs->setJavaEnabled(true);
	prefs->setJavaScriptEnabled(true);
	prefs->setJavaScriptCanOpenWindowsAutomatically(true);
	prefs->setDOMPasteAllowed(true);

	IWebPreferencesPrivate* privatePrefs = NULL;
	hr = prefs->QueryInterface(IID_IWebPreferencesPrivate, (void**) &privatePrefs);
	if (FAILED(hr) || !privatePrefs)
		HandleHResultError("Error getting IWebPreferencesPrivate", hr, true);

	privatePrefs->setDeveloperExtrasEnabled(host->DebugModeEnabled());
	privatePrefs->setDatabasesEnabled(true);
	privatePrefs->setLocalStorageEnabled(true);
	privatePrefs->setOfflineWebApplicationCacheEnabled(true);
	privatePrefs->setAllowUniversalAccessFromFileURLs(true);
	_bstr_t dbPath(FileUtils::GetApplicationDataDirectory(appid).c_str());
	privatePrefs->setLocalStorageDatabasePath(dbPath.copy());
	privatePrefs->Release();

	webView->setPreferences(prefs);
	prefs->Release();

	// allow app:// and ti:// to run with local permissions (cross-domain ajax,etc)
	_bstr_t appProto("app");
	webView->registerURLSchemeAsLocal(appProto.copy());

	_bstr_t tiProto("ti");
	webView->registerURLSchemeAsLocal(tiProto.copy());

	IWebViewPrivate *webViewPrivate;
	hr = webView->QueryInterface(IID_IWebViewPrivate, (void**) &webViewPrivate);
	if (FAILED(hr))
		HandleHResultError("Error getting IWebViewPrivate", hr);

	// Get the WebView's HWND
	hr = webViewPrivate->viewWindow((OLE_HANDLE*) &viewWindowHandle);
	if (FAILED(hr))
		HandleHResultError("Error getting WebView HWND", hr);

	// Get the WebView's WebInspector
	hr = webViewPrivate->inspector(&webInspector);
	if (FAILED(hr) || !webInspector)
		HandleHResultError("Error getting WebInspector HWND", hr);

	webViewPrivate->Release();

	_bstr_t inspector_url("ti://runtime/WebKit.resources/inspector/inspector.html");
	webInspector->setInspectorURL(inspector_url.copy());

	hr = webView->mainFrame(&mainFrame);
	if (FAILED(hr) || !webInspector)
		HandleHResultError("Error getting WebView main frame", hr);
}

Win32UserWindow::Win32UserWindow(WindowConfig* config, AutoUserWindow& parent) :
	UserWindow(config, parent),
	menu(0),
	activeMenu(0),
	nativeMenu(0),
	contextMenu(0),
	defaultIcon(0),
	webInspector(0),
	win32Host(Win32Host::Win32Instance())
{
	logger = Logger::Get("UI.Win32UserWindow");

}

Win32UserWindow::~Win32UserWindow()
{
	if (webView)
		webView->Release();

	if (mainFrame)
		mainFrame->Release();

	DestroyWindow(windowHandle);
}

std::string Win32UserWindow::GetTransparencyColor()
{
	char hexColor[7];
	sprintf(hexColor, "%2x%2x%2x", (int) GetRValue(transparencyColor),
		(int) GetGValue(transparencyColor), (int) GetBValue(transparencyColor));
	std::string color(hexColor);
	return color;
}

void Win32UserWindow::ResizeSubViews()
{
	logger->Debug("Called resize subviews");
	RECT rcClient;
	GetClientRect(windowHandle, &rcClient);
	MoveWindow(viewWindowHandle, 0, 0, rcClient.right, rcClient.bottom, TRUE);
}

HWND Win32UserWindow::GetWindowHandle()
{
	return this->windowHandle;
}

void Win32UserWindow::Hide()
{
	ShowWindow(windowHandle, SW_HIDE);
}

void Win32UserWindow::Show()
{
	ShowWindow(windowHandle, SW_SHOW);
}

void Win32UserWindow::Minimize()
{
	ShowWindow(windowHandle, SW_MINIMIZE);
}

void Win32UserWindow::Unminimize()
{
	ShowWindow(windowHandle, SW_RESTORE);
}

bool Win32UserWindow::IsMinimized()
{
	return IsIconic(windowHandle) != 0;
}

void Win32UserWindow::Maximize()
{
	ShowWindow(windowHandle, SW_MAXIMIZE);
}

void Win32UserWindow::Unmaximize()
{
	ShowWindow(windowHandle, SW_RESTORE);
}

bool Win32UserWindow::IsMaximized()
{
	return IsZoomed(windowHandle) != 0;
}

void Win32UserWindow::Focus()
{
	SetFocus(viewWindowHandle);
}

void Win32UserWindow::Unfocus()
{
	// SetFocus sends a WM_KILLFOCUS message to the window that has focus.
	// By sending NULL, we basically turn off keystrokes to window that had focus.
	if (GetFocus() == windowHandle)
	{
		SetFocus(NULL);
	}
}

void Win32UserWindow::Open()
{
	this->InitWindow();
	this->SetTransparency(config->GetTransparency());	
	this->SetupDecorations();
	this->SetupPosition();
	this->SetupState();
	this->SetTopMost(config->IsTopMost() && config->IsVisible());
	this->InitWebKit();
	this->ResizeSubViews();

	// Ensure we have valid restore values
	restoreBounds = GetBounds();
	restoreStyles = GetWindowLong(windowHandle, GWL_STYLE);

	// Set this flag to indicate that when the frame is loaded we want to
	// show the window - we do this to prevent white screen while the first
	// URL loads in the WebView.
	this->requiresDisplay = true;

	// set initial window icon to icon associated with exe file
	char exePath[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL), exePath, MAX_PATH);
	defaultIcon = ExtractIconA(win32Host->GetInstanceHandle(), exePath, 0);
	if (defaultIcon)
	{
		SendMessageA(windowHandle, (UINT) WM_SETICON, ICON_BIG,
			(LPARAM) defaultIcon);
	}
	logger->Debug("Opening windowHandle=%i, viewWindowHandle=%i", 
		windowHandle,  viewWindowHandle);

	UpdateWindow(windowHandle);
	UpdateWindow(viewWindowHandle);

	ResizeSubViews();

	UserWindow::Open();
	this->SetURL(this->config->GetURL());
	if (!this->requiresDisplay)
	{
		Show();
		ShowWindow(viewWindowHandle, SW_SHOW);
	}

	SetupBounds();
	SetupState();
	FireEvent(Event::OPENED);
}

bool Win32UserWindow::Close()
{
	if (!this->active)
		return false;

	UserWindow::Close();

	// If the window is still active at this point, it
	// indicates an event listener has cancelled this close event.
	if (!this->active)
	{
		this->RemoveOldMenu();
		UserWindow::Closed();
	}

	return !this->active;
}

double Win32UserWindow::GetX()
{
	return GetBounds().x;
}

void Win32UserWindow::SetX(double x)
{
	this->SetupPosition();
}

double Win32UserWindow::GetY()
{
	return GetBounds().y;
}

void Win32UserWindow::SetY(double y)
{
	this->SetupPosition();
}

double Win32UserWindow::GetWidth()
{
	return GetBounds().width;
}

void Win32UserWindow::SetWidth(double width)
{
	this->SetupSize();
}

double Win32UserWindow::GetHeight()
{
	return GetBounds().height;
}

void Win32UserWindow::SetHeight(double height)
{
	this->SetupSize();
}

double Win32UserWindow::GetMaxWidth()
{
	return this->config->GetMaxWidth();
}

void Win32UserWindow::SetMaxWidth(double width)
{
	this->SetupSize();
}

double Win32UserWindow::GetMinWidth()
{
	return this->config->GetMinWidth();
}

void Win32UserWindow::SetMinWidth(double width)
{
	this->SetupSize();
}

double Win32UserWindow::GetMaxHeight()
{
	return this->config->GetMaxHeight();
}

void Win32UserWindow::SetMaxHeight(double height)
{
	this->SetupSize();
}

double Win32UserWindow::GetMinHeight()
{
	return this->config->GetMinHeight();
}

void Win32UserWindow::SetMinHeight(double height)
{
	this->SetupSize();
}

Bounds Win32UserWindow::GetBounds()
{
	Bounds bounds;

	RECT rect, windowRect;
	GetWindowRect(windowHandle, &windowRect);
	GetClientRect(windowHandle, &rect);

	bounds.x = windowRect.left;
	bounds.y = windowRect.top;
	bounds.width = rect.right - rect.left;
	bounds.height = rect.bottom - rect.top;
	
	return bounds;
}

void Win32UserWindow::SetupBounds()
{
	Bounds bounds;
	bounds.x = this->config->GetX();
	bounds.y = this->config->GetY();
	bounds.width = this->config->GetWidth();
	bounds.height = this->config->GetHeight();
	this->SetBounds(bounds);
}

void Win32UserWindow::SetBounds(Bounds bounds)
{
	HWND desktop = GetDesktopWindow();
	RECT desktopRect, boundsRect;

	GetWindowRect(desktop, &desktopRect);
	if (bounds.x == UIBinding::CENTERED)
	{
		bounds.x = (desktopRect.right - bounds.width) / 2;
		this->config->SetX(bounds.x);
	}
	if (bounds.y == UIBinding::CENTERED)
	{
		bounds.y = (desktopRect.bottom - bounds.height) / 2;
		this->config->SetY(bounds.y);
	}

	UINT flags = SWP_SHOWWINDOW | SWP_NOZORDER;
	if (!this->config->IsVisible())
	{
		flags = SWP_HIDEWINDOW;
	}

	boundsRect.left = bounds.x;
	boundsRect.right = bounds.x + bounds.width;
	boundsRect.top = bounds.y;
	boundsRect.bottom = bounds.y + bounds.height;
	
	if (this->config->IsUsingChrome())
	{
		AdjustWindowRect(&boundsRect, GetWindowLong(windowHandle, GWL_STYLE), 
			!menu.isNull());
		this->chromeWidth = boundsRect.right - boundsRect.left - (int)bounds.width;
		this->chromeHeight = boundsRect.bottom - boundsRect.top - (int)bounds.height;
	}
	else
	{
		this->chromeWidth = 0;
		this->chromeHeight = 0;
	}

	SetWindowPos(windowHandle, NULL, bounds.x, bounds.y,
		bounds.width + chromeWidth, bounds.height + chromeHeight, flags);
}

void Win32UserWindow::SetTitleImpl(std::string& title)
{
	std::wstring titleW = ::UTF8ToWide(title);
	SetWindowTextW(windowHandle, titleW.c_str());
}

void Win32UserWindow::SetURL(std::string& url_)
{
	std::string url = URLUtils::NormalizeURL(url_);
	Win32UIBinding::SetProxyForURL(url);

	IWebMutableURLRequest* request = 0;
	std::wstring method = L"GET" ;

	HRESULT hr = WebKitCreateInstance(CLSID_WebMutableURLRequest, 0, 
		IID_IWebMutableURLRequest, (void**) &request);
	if (FAILED(hr))
		HandleHResultError("Error creating WebMutableURLRequest", hr, true);

	std::wstring wurl = ::UTF8ToWide(url);
	hr = request->initWithURL(SysAllocString(wurl.c_str()), 
		WebURLRequestUseProtocolCachePolicy, 60);
	if (FAILED(hr))
	{
		request->Release();
		std::string error("Error initialiazing WebMutableURLRequest for ");
		error.append(url);
		HandleHResultError(error, hr, true);
	}

	hr = request->setHTTPMethod(SysAllocString(method.c_str()));
	if (FAILED(hr))
	{
		request->Release();
		std::string error("Error setting HTTP method for ");
		error.append(url);
		HandleHResultError(error, hr, true);
	}

	hr = mainFrame->loadRequest(request);
	if (FAILED(hr))
	{
		request->Release();
		std::string error("Error starting load request for ");
		error.append(url);
		HandleHResultError(error, hr, true);
	}

	SetFocus(viewWindowHandle);
}

void Win32UserWindow::SetResizable(bool resizable)
{
	this->SetupDecorations();
	this->SetupSize();
}

void Win32UserWindow::SetMaximizable(bool maximizable)
{
	this->SetupDecorations();
}

void Win32UserWindow::SetMinimizable(bool minimizable)
{
	this->SetupDecorations();
}

void Win32UserWindow::SetCloseable(bool closeable)
{
	this->SetupDecorations();
}

bool Win32UserWindow::IsVisible()
{
	return IsWindowVisible(windowHandle);
}

void Win32UserWindow::SetTransparency(double transparency)
{
	SetWindowLong(windowHandle, GWL_EXSTYLE, GetStyleFromConfig());
	SetLayeredWindowAttributes(windowHandle, 0,
		(BYTE) floor(config->GetTransparency() * 255), LWA_ALPHA);
}

void Win32UserWindow::SetFullscreen(bool fullscreen)
{
	if (fullscreen)
	{
		restoreBounds = GetBounds();
		restoreStyles = GetWindowLong(windowHandle, GWL_STYLE);

		HMONITOR hmon = MonitorFromWindow(this->windowHandle,
				MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		if (GetMonitorInfo(hmon, &mi))
		{
			SetWindowLong(windowHandle, GWL_STYLE, 0);
			SetWindowPos(windowHandle, NULL, 0, 0, mi.rcMonitor.right
					- mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_SHOWWINDOW);
		}

		FireEvent(Event::FULLSCREENED);
	}
	else
	{
		SetWindowLong(windowHandle, GWL_STYLE, restoreStyles);
		SetBounds(restoreBounds);
		FireEvent(Event::UNFULLSCREENED);
	}
}

void Win32UserWindow::SetMenu(AutoMenu menu)
{
	this->menu = menu.cast<Win32Menu>();
	this->SetupMenu();
}

AutoMenu Win32UserWindow::GetMenu()
{
	return this->menu;
}

void Win32UserWindow::SetContextMenu(AutoMenu menu)
{
	this->contextMenu = menu.cast<Win32Menu>();
}

AutoMenu Win32UserWindow::GetContextMenu()
{
	return this->contextMenu;
}

void Win32UserWindow::SetIcon(std::string& iconPath)
{
	this->iconPath = iconPath;
	this->SetupIcon();
}

void Win32UserWindow::SetupIcon()
{
	std::string iconPath = this->iconPath;

	if (iconPath.empty()) {
		Win32UIBinding* b = static_cast<Win32UIBinding*>(UIBinding::GetInstance());
		iconPath = b->GetIcon();
	}

	HICON icon = defaultIcon;
	if (!iconPath.empty()) {
		icon = Win32UIBinding::LoadImageAsIcon(iconPath, 32, 32);
	}
	if (!icon) { // Icon failed to load
		icon = defaultIcon;
	}
	SendMessageA(windowHandle, (UINT) WM_SETICON, ICON_BIG, (LPARAM) icon);
}

std::string& Win32UserWindow::GetIcon()
{
	return iconPath;
}

void Win32UserWindow::SetUsingChrome(bool chrome)
{
	this->SetupDecorations();
}

void Win32UserWindow::SetupDecorations()
{
	long windowStyle = GetWindowLong(this->windowHandle, GWL_STYLE);

	SetFlag(windowStyle, WS_OVERLAPPED, config->IsUsingChrome());
	SetFlag(windowStyle, WS_SIZEBOX,
		config->IsUsingChrome() && config->IsResizable());
	SetFlag(windowStyle, WS_OVERLAPPEDWINDOW,
		config->IsUsingChrome() && config->IsResizable());
	SetFlag(windowStyle, WS_SYSMENU,
		config->IsUsingChrome() && config->IsCloseable());
	SetFlag(windowStyle, WS_CAPTION, config->IsUsingChrome());
	SetFlag(windowStyle, WS_BORDER, config->IsUsingChrome());
	SetFlag(windowStyle, WS_MAXIMIZEBOX, config->IsMaximizable());
	SetFlag(windowStyle, WS_MINIMIZEBOX, config->IsMinimizable());

	SetWindowLong(this->windowHandle, GWL_STYLE, windowStyle);

	if (this->active && config->IsVisible())
	{
		Hide();
		Show();
	}
}

void Win32UserWindow::SetupState()
{
	if (config->IsFullscreen())
	{
		this->SetFullscreen(true);
	}
	else if (config->IsMaximized())
	{
		this->Maximize();
	}
	else if (config->IsMinimized())
	{
		this->Minimize();
	}
}

void Win32UserWindow::AppMenuChanged()
{
	if (this->menu.isNull())
	{
		this->SetupMenu();
	}
}

void Win32UserWindow::AppIconChanged()
{
	this->SetupIcon();
}

void Win32UserWindow::RemoveOldMenu()
{
	if (!this->activeMenu.isNull() && this->nativeMenu) {
		this->activeMenu->DestroyNative(this->nativeMenu);
	}

	if (this->windowHandle != NULL && this->nativeMenu) {
		::SetMenu(this->windowHandle, NULL);
	}

	this->activeMenu = 0;
	this->nativeMenu = 0;

}

void Win32UserWindow::SetupMenu()
{
	AutoPtr<Win32Menu> menu = this->menu;

	// No window menu, try to use the application menu.
	if (menu.isNull())
	{
		Win32UIBinding* b = static_cast<Win32UIBinding*>(UIBinding::GetInstance());
		menu = b->GetMenu().cast<Win32Menu>();
	}

	// Only do this if the menu is actually changing.
	if (menu.get() != this->activeMenu.get()) {

		if (!menu.isNull() && this->windowHandle) {
			this->RemoveOldMenu();

			HMENU newNativeMenu = menu->CreateNativeTopLevel(true);
			::SetMenu(this->windowHandle, newNativeMenu);
			this->nativeMenu = newNativeMenu;
		}
		this->activeMenu = menu;
	}
}

// called by frame load delegate to let the window know it's loaded
void Win32UserWindow::FrameLoaded()
{
	if (this->requiresDisplay && this->config->IsVisible())
	{
		this->requiresDisplay = false;
		this->Show();
	}
}

bool Win32UserWindow::IsTopMost()
{
	return this->config->IsTopMost();
}

void Win32UserWindow::SetTopMost(bool topmost)
{
	if (topmost)
	{
		SetWindowPos(windowHandle, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		SetWindowPos(windowHandle, HWND_NOTOPMOST, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE);
	}
}

void Win32UserWindow::SetupPosition()
{
	Bounds b = GetBounds();
	b.x = this->config->GetX();
	b.y = this->config->GetY();
	b.width = this->config->GetWidth();
	b.height = this->config->GetHeight();	
	this->SetBounds(b);
}


void Win32UserWindow::SetupSize()
{
	Bounds b = GetBounds();
	b.width = this->config->GetWidth();
	b.height = this->config->GetHeight();

	this->SetBounds(b);
}

void Win32UserWindow::ShowInspector(bool console)
{
	if (this->webInspector)
	{
		if (console)
		{
			this->webInspector->showConsole();
		}
		else
		{
			this->webInspector->show();
		}
	}
}

void Win32UserWindow::OpenFileChooserDialog(
	KMethodRef callback,
	bool multiple,
	std::string& title,
	std::string& path,
	std::string& defaultName,
	std::vector<std::string>& types,
	std::string& typesDescription)
{

	KListRef results = this->SelectFile(
		false, multiple, title, path, defaultName, types, typesDescription);
	callback->Call(ValueList(Value::NewList(results)));
}

void Win32UserWindow::OpenFolderChooserDialog(
	KMethodRef callback,
	bool multiple,
	std::string& title,
	std::string& path,
	std::string& defaultName)
{
	KListRef results = SelectDirectory(multiple, title, path, defaultName);
	callback->Call(ValueList(Value::NewList(results)));
}

void Win32UserWindow::OpenSaveAsDialog(
	KMethodRef callback,
	std::string& title,
	std::string& path,
	std::string& defaultName,
	std::vector<std::string>& types,
	std::string& typesDescription)
{
	KListRef results = SelectFile(
		true, false, title, path, defaultName, types, typesDescription);
	callback->Call(ValueList(Value::NewList(results)));
}

KListRef Win32UserWindow::SelectFile(
 	bool saveDialog,
	bool multiple,
	std::string& title,
	std::string& path,
	std::string& defaultName,
	std::vector<std::string>& types,
	std::string& typesDescription)
{
	std::wstring filter;
	std::wstring typesDescriptionW = ::UTF8ToWide(typesDescription);
	if (types.size() > 0)
	{
		//"All\0*.*\0Test\0*.TXT\0";
		if (typesDescription.size() == 0)
		{
			// Reasonable default?
			typesDescriptionW = L"Selected Files";
		}
		filter.append(typesDescriptionW);
		filter.push_back(L'\0');
		
		for (int i = 0; i < types.size(); i++)
		{
			std::string type = types.at(i);
			std::wstring typeW = ::UTF8ToWide(type);
			//multiple filters: "*.TXT;*.DOC;*.BAK"
			size_t found = type.find("*.");
			if (found != 0)
			{
				filter.append(L"*.");
			}
			filter.append(typeW);
			filter.append(L";");
		}
		filter.push_back(L'\0');
	}

	OPENFILENAME ofn;
	wchar_t filenameW[1024];
	wcscpy(filenameW, ::UTF8ToWide(defaultName).c_str());
	
	// init OPENFILE
	std::wstring pathW = ::UTF8ToWide(path);
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->windowHandle;
	ofn.lpstrFile = filenameW;
	if (wcslen(filenameW) == 0)
	{
		ofn.lpstrFile[0] = L'\0';
	}
	ofn.nMaxFile = 1024;
	ofn.lpstrFilter = (LPWSTR) (filter.size() == 0 ? NULL : filter.c_str());
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = (LPWSTR) (pathW.length() == 0 ? NULL : pathW.c_str());
	ofn.Flags = OFN_EXPLORER;

	std::wstring titleW;
	if (!title.empty())
	{
		titleW = ::UTF8ToWide(title);
		ofn.lpstrTitle = titleW.c_str();
	}

	if (!saveDialog)
	{
		ofn.Flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	}

	if (multiple)
	{
		ofn.Flags |= OFN_ALLOWMULTISELECT;
	}

	KListRef results = new StaticBoundList();
	// display the open dialog box
	BOOL result;

	if (saveDialog)
	{
		result = ::GetSaveFileName(&ofn);
	}
	else
	{
		result = ::GetOpenFileName(&ofn);
	}

	if (result)
	{
		// if the user selected multiple files, ofn.lpstrFile is a NULL-separated list of filenames
		// if the user only selected one file, ofn.lpstrFile is a normal string

		std::vector<std::string> selectedFiles;
		ParseSelectedFiles(ofn.lpstrFile, selectedFiles);
		
		if (selectedFiles.size() == 1)
		{
			results->Append(Value::NewString(selectedFiles.at(0)));
		}
		else if (selectedFiles.size() > 1)
		{
			std::string directory(selectedFiles.at(0));
			for (int i = 1; i < selectedFiles.size(); i++)
			{
				std::string n;
				n.append(directory.c_str());
				n.append("\\");
				n.append(selectedFiles.at(i).c_str());
				results->Append(Value::NewString(n));
			}
		}
	}
	else
	{
		DWORD error = CommDlgExtendedError();
		std::string errorMessage = Win32Utils::QuickFormatMessage(error);
		Logger::Get("UI.Win32UserWindow")->Error("Error while opening files: %s", errorMessage.c_str());
	}
	return results;
}

KListRef Win32UserWindow::SelectDirectory(
	bool multiple,
	std::string& title,
	std::string& path,
	std::string& defaultName)
{
	KListRef results = new StaticBoundList();

	BROWSEINFO bi = { 0 };
	std::wstring titleW = ::UTF8ToWide(title);
	bi.lpszTitle = titleW.c_str();
	bi.hwndOwner = this->windowHandle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != 0)
	{
		wchar_t in_path[MAX_PATH];
		if (SHGetPathFromIDList(pidl, in_path))
		{
			std::wstring inPathW = in_path;
			std::string inPath = ::WideToUTF8(inPathW);
			results->Append(Value::NewString(inPath));
		}

		IMalloc * imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}
	}
	return results;
}

void Win32UserWindow::GetMinMaxInfo(MINMAXINFO* minMaxInfo)
{
	static int minYTrackSize = GetSystemMetrics(SM_CXMINTRACK);
	static int minXTrackSize = GetSystemMetrics(SM_CYMINTRACK);
	int maxWidth = (int) GetMaxWidth();
	int minWidth = (int) GetMinWidth();
	int maxHeight = (int) GetMaxHeight();
	int minHeight = (int) GetMinHeight();

	// offset the size of the window chrome
	if (IsUsingChrome()) 
	{
		if (maxWidth > -1)
			maxWidth += chromeWidth;
		if (minWidth > -1)
			minWidth += chromeWidth;
		
		if (maxHeight > -1)
			maxHeight += chromeHeight;
		if (minHeight > -1)
			minHeight += chromeHeight;
	}

	minMaxInfo->ptMaxTrackSize.x = maxWidth == -1 ? INT_MAX : maxWidth;
	minMaxInfo->ptMinTrackSize.x = minWidth == -1 ? minXTrackSize : minWidth;
	minMaxInfo->ptMaxTrackSize.y = maxHeight == -1 ? INT_MAX : maxHeight;
	minMaxInfo->ptMinTrackSize.y = minHeight == -1 ? minYTrackSize : minHeight;
}

void Win32UserWindow::ParseSelectedFiles(
	const wchar_t *s, std::vector<std::string> &selectedFiles)
{
	std::string selectedFile;
	
	// input string is expected to be composed of single-NULL-separated tokens, and double-NULL terminated
	int i = 0;
	while (true)
	{
		wchar_t c;

		c = s[i++];

		if (c == L'\0')
		{
			// finished reading a token, save it in tokens vectory
			selectedFiles.push_back(selectedFile);
			selectedFile.clear();

			if (!FileUtils::IsDirectory(selectedFiles.at(0)) && selectedFiles.size() == 1)
			{
				// The first entry is a file, and not a directory
				// This means we should only have 1 file in this selection
				break;
			}
			c = s[i]; // don't increment index because next token loop needs to read this char again

			// if next char is NULL, then break out of the while loop
			if (c == L'\0')
			{
				break; // out of while loop
			}
			else
			{
				continue; // read next token
			}
		}

		selectedFile.push_back(c);
	}
}

void Win32UserWindow::RedrawMenu()
{
	if (this->windowHandle) {
		DrawMenuBar(this->windowHandle);
	}
}

/*static*/
void Win32UserWindow::RedrawAllMenus()
{
	// Notify all windows that the app menu has changed.
	std::vector<AutoUserWindow>& windows = UIBinding::GetInstance()->GetOpenWindows();
	std::vector<AutoUserWindow>::iterator i = windows.begin();
	while (i != windows.end())
	{
		AutoPtr<Win32UserWindow> wuw = (*i++).cast<Win32UserWindow>();
		if (!wuw.isNull())
			wuw->RedrawMenu();
	}
}

