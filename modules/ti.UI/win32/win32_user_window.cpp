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
	return reinterpret_cast<void*> (SetWindowLongPtr(hwnd, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR> (userData)));
}

static void* GetWindowUserData(HWND hWnd)
{
	return reinterpret_cast<void*> (GetWindowLongPtr(hWnd, GWLP_USERDATA));
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
	Win32UserWindow *window = Win32UserWindow::FromWindow(hWnd);

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
				MINMAXINFO *mmi = (MINMAXINFO*) lParam;
				static int minYTrackSize = GetSystemMetrics(SM_CXMINTRACK);
				static int minXTrackSize = GetSystemMetrics(SM_CYMINTRACK);
				int maxWidth = (int) window->GetMaxWidth();
				int minWidth = (int) window->GetMinWidth();
				int maxHeight = (int) window->GetMaxHeight();
				int minHeight = (int) window->GetMinHeight();

				// offset the size of the window chrome
				if (window->IsUsingChrome()) 
				{
					if (maxWidth > -1)
						maxWidth += window->chromeWidth;
					if (minWidth > -1)
						minWidth += window->chromeWidth;
					
					if (maxHeight > -1)
						maxHeight += window->chromeHeight;
					if (minHeight > -1)
						minHeight += window->chromeHeight;
				}

				mmi->ptMaxTrackSize.x = maxWidth == -1 ? INT_MAX : maxWidth;
				mmi->ptMinTrackSize.x = minWidth == -1 ? minXTrackSize : minWidth;
				mmi->ptMaxTrackSize.y = maxHeight == -1 ? INT_MAX : maxHeight;
				mmi->ptMinTrackSize.y = minHeight == -1 ? minYTrackSize : minHeight;
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
			window->FireEvent(Event::FOCUSED);
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_KILLFOCUS:
			window->FireEvent(Event::UNFOCUSED);
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

			if (itemId == WEB_INSPECTOR_MENU_ITEM_ID) {
				Win32UserWindow* wuw = Win32UserWindow::FromWindow(hWnd);
				if (wuw)
					wuw->ShowInspector(false);
				break;

			} else if (Win32MenuItem::HandleClickEvent(nativeMenu, position)) {
				break;
			} else {
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void Win32UserWindow::InitWindow()
{
	Win32UserWindow::RegisterWindowClass(win32Host->GetInstanceHandle());

	std::wstring titleW = UTF8ToWide(config->GetTitle());
	this->windowHandle = CreateWindowExW(
		WS_EX_APPWINDOW /*WS_EX_LAYERED*/, 
		USERWINDOW_WINDOW_CLASS,
		titleW.c_str(),
		WS_CLIPCHILDREN, CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL,
		win32Host->GetInstanceHandle(), NULL);

	if (this->windowHandle == NULL)
	{
		std::ostringstream error;
		error << "Error Creating Window: " << GetLastError();
		logger->Error(error.str());
	}


	// these APIs are semi-private -- we probably shouldn't mark them
	// make our HWND available to 3rd party devs without needing our headers
	SharedValue windowHandle = Value::NewVoidPtr((void*) this->windowHandle);
	this->Set("windowHandle", windowHandle);
	logger->Debug("Initializing windowHandle: %i", windowHandle);

	SetWindowUserData(this->windowHandle, this);
}

void Win32UserWindow::InitWebKit()
{
	HRESULT hr = WebKitCreateInstance(CLSID_WebView, 0,
		 IID_IWebView, (void**) &(this->webView));
	
	if (FAILED(hr))
	{
		std::string createError("Error creating WebKitWebView: ");
		switch (hr)
		{
			case REGDB_E_CLASSNOTREG: 
				createError += "REGDB_E_CLASSNOTREG";
				break;
			case CLASS_E_NOAGGREGATION: 
				createError += "CLASS_E_NOAGGREGATION"; 
				break;
			case E_NOINTERFACE: 
				createError += "E_NOINTERFACE"; 
				break;
			case E_UNEXPECTED: 
				createError += "E_UNEXPECTED";
				break;
			case E_OUTOFMEMORY: 
				createError += "E_OUTOFMEMORY";
				break;
			case E_INVALIDARG:
				createError += "E_INVALIDARG";
				break;
			default:
				createError += "Unknown Error: ";
				break;
		}
		logger->Error(createError.c_str());
		MessageBoxA(NULL, createError.c_str(), "Error Initializing WebKit", MB_ICONERROR | MB_OK);
		exit(-1);
	}

	// set the custom user agent for Titanium
	const char *version = host->GetGlobalObject()->Get("version")->ToString();
	char userAgent[128];
	//TI-303 we need to add safari UA to our UA to resolve broken
	//sites that look at Safari and not WebKit for UA
	sprintf(userAgent, "Version/4.0 Safari/528.16 %s/%s", PRODUCT_NAME, version);
	_bstr_t ua(userAgent);
	webView->setApplicationNameForUserAgent(ua.copy());

	// place our user agent string in the global so we can later use it
	SharedKObject global = host->GetGlobalObject();
	_bstr_t uaURL("http://titaniumapp.com");
	BSTR uaResp;
	webView->userAgentForURL(uaURL.copy(), &uaResp);
	std::string uaStr = _bstr_t(uaResp);
	global->Set("userAgent", Value::NewString(uaStr.c_str()));

	logger->Debug("create frame load delegate ");
	frameLoadDelegate = new Win32WebKitFrameLoadDelegate(this);
	uiDelegate = new Win32WebKitUIDelegate(this);
	policyDelegate = new Win32WebKitPolicyDelegate(this);
	
	logger->Debug("set delegates, set host window");
	hr = webView->setFrameLoadDelegate(frameLoadDelegate);
	hr = webView->setUIDelegate(uiDelegate);
	hr = webView->setPolicyDelegate(policyDelegate);
	hr = webView->setHostWindow((OLE_HANDLE) windowHandle);
	
	logger->Debug("init with frame");
	RECT clientRect;
	GetClientRect(windowHandle, &clientRect);
	hr = webView->initWithFrame(clientRect, 0, 0);

	AppConfig *appConfig = AppConfig::Instance();
	std::string appid = appConfig->GetAppID();

	IWebPreferences *prefs = NULL;
	hr = WebKitCreateInstance(CLSID_WebPreferences, 0,
		IID_IWebPreferences, (void**) &prefs);
	if (FAILED(hr) || prefs == NULL)
	{
		logger->Error("Couldn't create the web preferences object");
	}
	else
	{
		_bstr_t pi(appid.c_str());
		prefs->initWithIdentifier(pi.copy(), &prefs);

		prefs->setCacheModel(WebCacheModelDocumentBrowser);
		prefs->setPlugInsEnabled(true);
		prefs->setJavaEnabled(true);
		prefs->setJavaScriptEnabled(true);
		prefs->setJavaScriptCanOpenWindowsAutomatically(true);
		prefs->setDOMPasteAllowed(true);

		IWebPreferencesPrivate* privatePrefs = NULL;
		hr = prefs->QueryInterface(IID_IWebPreferencesPrivate,
				(void**) &privatePrefs);
		if (FAILED(hr))
		{
			logger->Error("Failed to get private preferences");
		} else {
			privatePrefs->setDeveloperExtrasEnabled(host->IsDebugMode());
			//privatePrefs->setDeveloperExtrasEnabled(host->IsDebugMode());
			privatePrefs->setDatabasesEnabled(true);
			privatePrefs->setLocalStorageEnabled(true);
			privatePrefs->setOfflineWebApplicationCacheEnabled(true);

			_bstr_t dbPath(
					FileUtils::GetApplicationDataDirectory(appid).c_str());
			privatePrefs->setLocalStorageDatabasePath(dbPath.copy());
			privatePrefs->Release();
		}

		webView->setPreferences(prefs);
		prefs->Release();
	}

	// allow app:// and ti:// to run with local permissions (cross-domain ajax,etc)
	_bstr_t appProto("app");
	webView->registerURLSchemeAsLocal(appProto.copy());

	_bstr_t tiProto("ti");
	webView->registerURLSchemeAsLocal(tiProto.copy());

	IWebViewPrivate *webViewPrivate;
	hr = webView->QueryInterface(IID_IWebViewPrivate,
			(void**) &webViewPrivate);
	hr = webViewPrivate->viewWindow((OLE_HANDLE*) &viewWindowHandle);

	hr = webViewPrivate->inspector(&webInspector);
	if (FAILED(hr) || webInspector == NULL)
	{
		logger->Error("Couldn't retrieve the web inspector object");
	}

	webViewPrivate->Release();

	_bstr_t inspector_url("ti://runtime/WebKit.resources/inspector/inspector.html");
	webInspector->setInspectorURL(inspector_url.copy());

	hr = webView->mainFrame(&webFrame);
	//webView->setShouldCloseWithWindow(TRUE);
}

Win32UserWindow::Win32UserWindow(WindowConfig* config, AutoUserWindow& parent) :
	UserWindow(config, parent),
	menu(0),
	activeMenu(0),
	nativeMenu(0),
	contextMenu(0),
	defaultIcon(0),
	webInspector(0)
{
	logger = Logger::Get("UI.Win32UserWindow");
	
	win32Host = static_cast<kroll::Win32Host*>(binding->GetHost());
	this->InitWindow();

	this->ReloadTiWindowConfig();
	this->SetupDecorations(false);

	Bounds b;
	b.x = config->GetX();
	b.y = config->GetY();
	b.width = config->GetWidth();
	b.height = config->GetHeight();
	SetBounds(b);

	this->InitWebKit();
	
	//webView = WebView::createInstance();
	logger->Debug("resize subviews");
	ResizeSubViews();

	// ensure we have valid restore values
	restoreBounds = GetBounds();
	restoreStyles = GetWindowLong(windowHandle, GWL_STYLE);

	if (this->config->IsFullscreen())
	{
		this->SetFullscreen(true);
	}
	else if (this->config->IsMaximized())
	{
		this->Maximize();
	}
	else if (this->config->IsMinimized())
	{
		this->Minimize();
	}

	if (this->config->IsTopMost() && this->config->IsVisible())
	{
		this->SetTopMost(true);
	}

	// set this flag to indicate that when the frame is loaded
	// we want to show the window - we do this to prevent white screen
	// while the URL is being fetched
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

	if (config->GetTransparency() < 1.0)
	{
		SetWindowLong( this->windowHandle, GWL_EXSTYLE, WS_EX_LAYERED);
		SetLayeredWindowAttributes(this->windowHandle, 0, (BYTE) floor(
				config->GetTransparency() * 255), LWA_ALPHA);
		SetLayeredWindowAttributes(this->windowHandle, transparencyColor, 0,
			LWA_COLORKEY);
	}
}

Win32UserWindow::~Win32UserWindow()
{
	if (webView)
		webView->Release();

	if (webFrame)
		webFrame->Release();

	DestroyWindow(windowHandle);
}

std::string Win32UserWindow::GetTransparencyColor()
{
	char hexColor[7];
	sprintf(hexColor, "%2x%2x%2x", (int) GetRValue(transparencyColor),
			(int) GetGValue(transparencyColor), (int) GetBValue(
					transparencyColor));

	std::string color(hexColor);

	return color;
}

void Win32UserWindow::ResizeSubViews()
{
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
	SetFocus(windowHandle);
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
	logger->Debug("Opening windowHandle=%i, viewWindowHandle=%i", windowHandle,  viewWindowHandle);

	UpdateWindow(windowHandle);
	UpdateWindow(viewWindowHandle);

	ResizeSubViews();

	UserWindow::Open();
	SetURL(this->config->GetURL());
	if (!this->requiresDisplay)
	{
		ShowWindow(windowHandle, SW_SHOW);
		ShowWindow(viewWindowHandle, SW_SHOW);
	}
	
	this->SetupBounds();
	if (this->config->IsMaximized()) {
		this->Maximize();
	}
	else if (this->config->IsMinimized()) {
		this->Minimize();
	}
	
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
		AdjustWindowRect(&boundsRect, GetWindowLong(windowHandle, GWL_STYLE), !menu.isNull());
		this->chromeWidth = boundsRect.right - boundsRect.left - (int)bounds.width;
		this->chromeHeight = boundsRect.bottom - boundsRect.top - (int)bounds.height;
	}
	else
	{
		this->chromeWidth = 0;
		this->chromeHeight = 0;
	}
	
	SetWindowPos(windowHandle, NULL, bounds.x, bounds.y, bounds.width + chromeWidth, bounds.height + chromeHeight, flags);
}

void Win32UserWindow::SetTitleImpl(std::string& title)
{
	std::wstring titleW = UTF8ToWide(title);
	SetWindowTextW(windowHandle, titleW.c_str());
}

void Win32UserWindow::SetURL(std::string& url_)
{
	std::string url = URLUtils::NormalizeURL(url_);
	Win32UIBinding::SetProxyForURL(url);

	IWebMutableURLRequest* request = 0;
	std::wstring method = L"GET" ;

	//if (url.length() > 0 && (PathFileExists(url.c_str()) || PathIsUNC(url.c_str())))
	//{
	//	TCHAR fileURL[INTERNET_MAX_URL_LENGTH];
	//	DWORD fileURLLength = sizeof(fileURL)/sizeof(fileURL[0]);
	//	if (SUCCEEDED(UrlCreateFromPath(url.c_str(), fileURL, &fileURLLength, 0)))
	//		url = fileURL;
	//}
	std::wstring wurl = UTF8ToWide(url);

	logger->Debug("CoCreateInstance");
	HRESULT hr = WebKitCreateInstance(CLSID_WebMutableURLRequest, 0, 
		IID_IWebMutableURLRequest, (void**) &request);
	if (FAILED(hr))
		goto exit;

	logger->Debug("initWithURL: %s", url.c_str());
	hr = request->initWithURL(SysAllocString(wurl.c_str()), WebURLRequestUseProtocolCachePolicy, 60);
	if (FAILED(hr))
		goto exit;

	logger->Debug("set HTTP method");
	hr = request->setHTTPMethod(SysAllocString(method.c_str()));
	if (FAILED(hr))
		goto exit;

	logger->Debug("load request");
	hr = webFrame->loadRequest(request);
	if (FAILED(hr))
		goto exit;

	logger->Debug("set focus");
	SetFocus(viewWindowHandle);

exit:
	if (request)
		request->Release();
}

#define SetFlag(x,flag,b) ((b) ? x |= flag : x &= ~flag)
#define UnsetFlag(x,flag) (x &= ~flag)=

#define SetGWLFlag(wnd,flag,b) long window_style = GetWindowLong(wnd, GWL_STYLE);\
SetFlag(window_style, flag, b);\
SetWindowLong(wnd, GWL_STYLE, window_style);

void Win32UserWindow::SetResizable(bool resizable)
{
	SetGWLFlag(windowHandle, WS_SIZEBOX, this->config->IsUsingChrome() && resizable);
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
	if (config->GetTransparency() < 1.0)
	{
		SetWindowLong(this->windowHandle, GWL_EXSTYLE, WS_EX_LAYERED);
		SetLayeredWindowAttributes(this->windowHandle, 0, (BYTE) floor(
		config->GetTransparency() * 255), LWA_ALPHA);
	}
	else
	{
		SetWindowLong( this->windowHandle, GWL_EXSTYLE, WS_EX_APPWINDOW);
	}
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

void Win32UserWindow::SetupDecorations(bool showHide)
{
	long windowStyle = GetWindowLong(this->windowHandle, GWL_STYLE);

	SetFlag(windowStyle, WS_OVERLAPPED, config->IsUsingChrome());
	SetFlag(windowStyle, WS_CAPTION, config->IsUsingChrome());
	SetFlag(windowStyle, WS_SYSMENU, config->IsUsingChrome() && config->IsCloseable());
	SetFlag(windowStyle, WS_BORDER, config->IsUsingChrome());

	SetFlag(windowStyle, WS_MAXIMIZEBOX, config->IsMaximizable());
	SetFlag(windowStyle, WS_MINIMIZEBOX, config->IsMinimizable());

	SetWindowLong(this->windowHandle, GWL_STYLE, windowStyle);

	if (showHide && config->IsVisible())
	{
		ShowWindow(windowHandle, SW_HIDE);
		ShowWindow(windowHandle, SW_SHOW);
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

void Win32UserWindow::ReloadTiWindowConfig()
{
	//host->webview()->GetMainFrame()->SetAllowsScrolling(tiWindowConfig->isUsingScrollbars());
	//SetWindowText(hWnd, UTF8ToWide(tiWindowConfig->getTitle()).c_str());

	long windowStyle = GetWindowLong(this->windowHandle, GWL_STYLE);

	SetFlag(windowStyle, WS_MINIMIZEBOX, config->IsMinimizable());
	SetFlag(windowStyle, WS_MAXIMIZEBOX, config->IsMaximizable());

	SetFlag(windowStyle, WS_OVERLAPPEDWINDOW, config->IsUsingChrome() && config->IsResizable());
	SetFlag(windowStyle, WS_CAPTION, config->IsUsingChrome());

	SetWindowLong(this->windowHandle, GWL_STYLE, windowStyle);

	if (config->GetTransparency() < 1.0)
	{
		SetWindowLong( this->windowHandle, GWL_EXSTYLE, WS_EX_LAYERED);
		SetLayeredWindowAttributes(this->windowHandle, 0, (BYTE) floor(
				config->GetTransparency() * 255), LWA_ALPHA);
	}
	else
	{
		SetWindowLong( this->windowHandle, GWL_EXSTYLE, WS_EX_APPWINDOW);
	}
	
	SetLayeredWindowAttributes(this->windowHandle, transparencyColor, 0,
			LWA_COLORKEY);
}

// called by frame load delegate to let the window know it's loaded
void Win32UserWindow::FrameLoaded()
{
	if (this->requiresDisplay && this->config->IsVisible())
	{
		this->requiresDisplay = false;
		ShowWindow(windowHandle, SW_SHOW);
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
		SetWindowPos(windowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE
				| SWP_NOSIZE);
	}
	else
	{
		SetWindowPos(windowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE
				| SWP_NOSIZE);
	}
}

void Win32UserWindow::SetupPosition()
{
	Bounds b = GetBounds();
	b.x = this->config->GetX();
	b.y = this->config->GetY();
	
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
	SharedKMethod callback,
	bool multiple,
	std::string& title,
	std::string& path,
	std::string& defaultName,
	std::vector<std::string>& types,
	std::string& typesDescription)
{

	SharedKList results = this->SelectFile(
		false, multiple, title, path, defaultName, types, typesDescription);
	callback->Call(ValueList(Value::NewList(results)));
}

void Win32UserWindow::OpenFolderChooserDialog(
	SharedKMethod callback,
	bool multiple,
	std::string& title,
	std::string& path,
	std::string& defaultName)
{
	SharedKList results = SelectDirectory(multiple, title, path, defaultName);
	callback->Call(ValueList(Value::NewList(results)));
}

void Win32UserWindow::OpenSaveAsDialog(
	SharedKMethod callback,
	std::string& title,
	std::string& path,
	std::string& defaultName,
	std::vector<std::string>& types,
	std::string& typesDescription)
{
	SharedKList results = SelectFile(
		true, false, title, path, defaultName, types, typesDescription);
	callback->Call(ValueList(Value::NewList(results)));
}

SharedKList Win32UserWindow::SelectFile(
 	bool saveDialog,
	bool multiple,
	std::string& title,
	std::string& path,
	std::string& defaultName,
	std::vector<std::string>& types,
	std::string& typesDescription)
{
	std::wstring filter;
	std::wstring typesDescriptionW = UTF8ToWide(typesDescription);
	if (types.size() > 0)
	{
		//"All\0*.*\0Test\0*.TXT\0";
		filter.append(typesDescriptionW);
		filter.push_back(L'\0');
		for (int i = 0; i < types.size(); i++)
		{
			std::string type = types.at(i);
			std::wstring typeW = UTF8ToWide(type);
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
	wcscpy(filenameW, UTF8ToWide(defaultName).c_str());
	
	// init OPENFILE
	std::wstring pathW = UTF8ToWide(path);
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

	if (!title.empty())
	{
		std::wstring titleW = UTF8ToWide(title);
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

	SharedKList results = new StaticBoundList();
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

		std::vector<std::string> tokens;
		ParseStringNullSeparated(ofn.lpstrFile, tokens);

		if (tokens.size() == 1)
		{
			results->Append(Value::NewString(tokens.at(0)));
		}
		else if (tokens.size() > 1)
		{
			std::string directory(tokens.at(0));
			for (int i = 1; i < tokens.size(); i++)
			{
				std::string n;
				n.append(directory.c_str());
				n.append("\\");
				n.append(tokens.at(i).c_str());
				results->Append(Value::NewString(n));
			}
		}
	}
	else
	{
		DWORD error = CommDlgExtendedError();
		std::string errorMessage = Win32Utils::QuickFormatMessage(error);
		Logger::Get("UI.Win32UserWindow")->Error("Error while opening files: %s", errorMessage.c_str());
		/*
		printf("Error when opening files: %d\n", error);
		switch(error)
		{
			case CDERR_DIALOGFAILURE: printf("CDERR_DIALOGFAILURE\n"); break;
			case CDERR_FINDRESFAILURE: printf("CDERR_FINDRESFAILURE\n"); break;
			case CDERR_NOHINSTANCE: printf("CDERR_NOHINSTANCE\n"); break;
			case CDERR_INITIALIZATION: printf("CDERR_INITIALIZATION\n"); break;
			case CDERR_NOHOOK: printf("CDERR_NOHOOK\n"); break;
			case CDERR_LOCKRESFAILURE: printf("CDERR_LOCKRESFAILURE\n"); break;
			case CDERR_NOTEMPLATE: printf("CDERR_NOTEMPLATE\n"); break;
			case CDERR_LOADRESFAILURE: printf("CDERR_LOADRESFAILURE\n"); break;
			case CDERR_STRUCTSIZE: printf("CDERR_STRUCTSIZE\n"); break;
			case CDERR_LOADSTRFAILURE: printf("CDERR_LOADSTRFAILURE\n"); break;
			case FNERR_BUFFERTOOSMALL: printf("FNERR_BUFFERTOOSMALL\n"); break;
			case CDERR_MEMALLOCFAILURE: printf("CDERR_MEMALLOCFAILURE\n"); break;
			case FNERR_INVALIDFILENAME: printf("FNERR_INVALIDFILENAME\n"); break;
			case CDERR_MEMLOCKFAILURE: printf("CDERR_MEMLOCKFAILURE\n"); break;
			case FNERR_SUBCLASSFAILURE: printf("FNERR_SUBCLASSFAILURE\n"); break;
		}*/
	}
	return results;
}

SharedKList Win32UserWindow::SelectDirectory(
	bool multiple,
	std::string& title,
	std::string& path,
	std::string& defaultName)
{
	SharedKList results = new StaticBoundList();

	BROWSEINFO bi = { 0 };
	std::wstring titleW = UTF8ToWide(title);
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
			std::string inPath = WideToUTF8(inPathW);
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

void Win32UserWindow::ParseStringNullSeparated(
	const wchar_t *s, std::vector<std::string> &tokens)
{
	std::string token;

	// input string is expected to be composed of single-NULL-separated tokens, and double-NULL terminated
	int i = 0;
	while (true)
	{
		wchar_t c;

		c = s[i++];

		if (c == '\0')
		{
			// finished reading a token, save it in tokens vectory
			tokens.push_back(token);
			token.clear();

			c = s[i]; // don't increment index because next token loop needs to read this char again

			// if next char is NULL, then break out of the while loop
			if (c == '\0')
			{
				break; // out of while loop
			}
			else
			{
				continue; // read next token
			}
		}

		token.push_back(c);
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

