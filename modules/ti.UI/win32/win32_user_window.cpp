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
using namespace ti;

// slightly off white, there's probably a better way to do this
COLORREF transparencyColor = RGB(0xF9, 0xF9, 0xF9);

static void* SetWindowUserData(HWND hwnd, void* user_data)
{
	return reinterpret_cast<void*> (SetWindowLongPtr(hwnd, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR> (user_data)));
}

static void* GetWindowUserData(HWND hwnd)
{
	return reinterpret_cast<void*> (GetWindowLongPtr(hwnd, GWLP_USERDATA));
}

/*static*/
Win32UserWindow* Win32UserWindow::FromWindow(HWND hWnd)
{
	return reinterpret_cast<Win32UserWindow*> (GetWindowUserData(hWnd));
}

const TCHAR *windowClassName = "Win32UserWindow";

/*static*/
void Win32UserWindow::RegisterWindowClass(HINSTANCE hInstance)
{
	static bool class_initialized = false;
	if (!class_initialized)
	{
		//LoadString(hInstance, IDC_TIUSERWINDOW, windowClassName, 100);

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Win32UserWindow::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = 0;
		wcex.hIconSm = 0;
		wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
		//wcex.hbrBackground	= (HBRUSH)(COLOR_BACKGROUND+1);
		wcex.hbrBackground = CreateSolidBrush(transparencyColor);
		wcex.lpszMenuName = "";
		wcex.lpszClassName = windowClassName;

		ATOM result = RegisterClassEx(&wcex);
		if (result == NULL)
		{
			Logger::Get("UI.Win32UserWindow")->Error("Error Registering Window Class: %d", GetLastError());
		}

		class_initialized = true;
	}
}

void Win32UserWindow::AddMessageHandler(const ValueList& args,
		SharedValue result)
{
	if (args.size() < 2 || !args.at(0)->IsNumber() || !args.at(1)->IsMethod())
		return;

	long messageCode = (long) args.at(0)->ToDouble();
	SharedKMethod callback = args.at(1)->ToMethod();

	messageHandlers[messageCode] = callback;
}

/*static*/
LRESULT CALLBACK
Win32UserWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Win32UserWindow *window = Win32UserWindow::FromWindow(hWnd);

	if (window && (window->messageHandlers.size() > 0) && (window->messageHandlers.find(message) != window->messageHandlers.end()))
	{
		SharedKMethod handler = window->messageHandlers[message];
		ValueList args;
		args.push_back(Value::NewVoidPtr((void*)wParam));
		args.push_back(Value::NewVoidPtr((void*)lParam));
		handler->Call(args);
		return 0;
	}

	switch (message)
	{
		case WM_DESTROY:
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_CLOSE:
			window->Close();
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_GETMINMAXINFO:
			if (window) {
				MINMAXINFO *mmi = (MINMAXINFO*) lParam;
				static int minYTrackSize = GetSystemMetrics(SM_CXMINTRACK);
				static int minXTrackSize = GetSystemMetrics(SM_CYMINTRACK);
				int max_width = (int) window->GetMaxWidth();
				int min_width = (int) window->GetMinWidth();
				int max_height = (int) window->GetMaxHeight();
				int min_height = (int) window->GetMinHeight();

				// offset the size of the window chrome
				if (window->IsUsingChrome()) {
					if (max_width > -1)
						max_width += window->chromeWidth;
					if (min_width > -1)
						min_width += window->chromeWidth;
					
					if (max_height > -1)
						max_height += window->chromeHeight;
					if (min_height > -1)
						min_height += window->chromeHeight;
				}

				if (max_width == -1) {
					mmi->ptMaxTrackSize.x = INT_MAX; // Uncomfortably large
				} else {
					mmi->ptMaxTrackSize.x = max_width;
				}

				if (min_width == -1) {
					mmi->ptMinTrackSize.x = minXTrackSize;
				} else {
					mmi->ptMinTrackSize.x = min_width;
				}

				if (max_height == -1) {
					mmi->ptMaxTrackSize.y = INT_MAX; // Uncomfortably large
				} else {
					mmi->ptMaxTrackSize.y = max_height;
				}

				if (min_height == -1) {
					mmi->ptMinTrackSize.y = minYTrackSize;
				} else {
					mmi->ptMinTrackSize.y = min_height;
				}
			}
			break;

		case WM_SIZE:
			if (window->web_view)
			{
				window->ResizeSubViews();
				window->FireEvent(RESIZED);
				if (wParam == SIZE_MAXIMIZED) {
					window->FireEvent(MAXIMIZED);

				} else if (wParam == SIZE_MINIMIZED) {
					window->FireEvent(MINIMIZED);
				}
			}
			break;

		case WM_SETFOCUS:
			window->FireEvent(FOCUSED);
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_KILLFOCUS:
			window->FireEvent(UNFOCUSED);
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_MOVE:
			window->FireEvent(MOVED);
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_SHOWWINDOW:
			window->FireEvent(((BOOL)wParam) ? SHOWN : HIDDEN);
			return DefWindowProc(hWnd, message, wParam, lParam);

		case TI_TRAY_CLICKED: {
			UINT button = (UINT) lParam;
			if (button == WM_LBUTTONDOWN || button == WM_RBUTTONDOWN) {
				Win32TrayItem::HandleClickEvent(hWnd, message, wParam, lParam);
			}
		} break;

		case WM_MENUCOMMAND: {
			HMENU nativeMenu = (HMENU) lParam;
			UINT position = (UINT) wParam;
			UINT itemId = GetMenuItemID(nativeMenu, position);

			if (itemId == WEB_INSPECTOR_MENU_ITEM_ID) {
				Win32UserWindow* wuw = Win32UserWindow::FromWindow(hWnd);
				if (wuw)
					wuw->ShowWebInspector();
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
	Win32UserWindow::RegisterWindowClass(win32_host->GetInstanceHandle());
	this->window_handle
			= CreateWindowEx(WS_EX_APPWINDOW /*WS_EX_LAYERED*/, windowClassName,
					config->GetTitle().c_str(), WS_CLIPCHILDREN, CW_USEDEFAULT,
					0, CW_USEDEFAULT, 0, NULL, NULL,
					win32_host->GetInstanceHandle(), NULL);

	if (this->window_handle == NULL)
	{
		std::ostringstream error;
		error << "Error Creating Window: " << GetLastError();
		logger->Error(error.str());
	}

	logger->Debug("Initializing window_handle: %i", window_handle);
	// make our HWND available to 3rd party devs without needing our headers
	SharedValue windowHandle = Value::NewVoidPtr((void*) this->window_handle);
	// these APIs are semi-private -- we probably shouldn't mark them
	this->Set("windowHandle", windowHandle);
	this->SetMethod("addMessageHandler", &Win32UserWindow::AddMessageHandler);
	
	SetWindowUserData(this->window_handle, this);
}

void Win32UserWindow::InitWebKit()
{
	HRESULT hr = CoCreateInstance(CLSID_WebView, 0, CLSCTX_ALL, IID_IWebView,
			(void**) &(this->web_view));
	
	if (FAILED(hr))
	{
		std::ostringstream createError;
		createError << "Error Creating WebView: ";
		switch (hr) {
			case REGDB_E_CLASSNOTREG: createError << "REGDB_E_CLASSNOTREG"; break;
			case CLASS_E_NOAGGREGATION: createError << "CLASS_E_NOAGGREGATION"; break;
			case E_NOINTERFACE: createError << "E_NOINTERFACE"; break;
			case E_UNEXPECTED: createError << "E_UNEXPECTED"; break;
			case E_OUTOFMEMORY: createError << "E_OUTOFMEMORY"; break;
			case E_INVALIDARG: createError << "E_INVALIDARG"; break;
			default: createError << "Unknown Error: " << hr; break;
		}
		logger->Error(createError.str());
		MessageBox(NULL, createError.str().c_str(), "Error Initializing WebKit", MB_ICONERROR | MB_OK);
		exit(-1);
	}

	// set the custom user agent for Titanium
	const char *version = host->GetGlobalObject()->Get("version")->ToString();
	char userAgent[128];
	//TI-303 we need to add safari UA to our UA to resolve broken
	//sites that look at Safari and not WebKit for UA
	sprintf(userAgent, "Version/4.0 Safari/528.16 %s/%s", PRODUCT_NAME, version);
	_bstr_t ua(userAgent);
	web_view->setApplicationNameForUserAgent(ua.copy());

	// place our user agent string in the global so we can later use it
	SharedKObject global = host->GetGlobalObject();
	_bstr_t uaurl("http://titaniumapp.com");
	BSTR uaresp;
	web_view->userAgentForURL(uaurl.copy(), &uaresp);
	std::string ua_str = _bstr_t(uaresp);
	global->Set("userAgent", Value::NewString(ua_str.c_str()));

	logger->Debug("create frame load delegate ");
	frameLoadDelegate = new Win32WebKitFrameLoadDelegate(this);
	uiDelegate = new Win32WebKitUIDelegate(this);
	policyDelegate = new Win32WebKitPolicyDelegate(this);

	logger->Debug("set delegates, set host window");
	hr = web_view->setFrameLoadDelegate(frameLoadDelegate);
	hr = web_view->setUIDelegate(uiDelegate);
	hr = web_view->setPolicyDelegate(policyDelegate);
	hr = web_view->setHostWindow((OLE_HANDLE) window_handle);

	logger->Debug("init with frame");
	RECT client_rect;
	GetClientRect(window_handle, &client_rect);
	hr = web_view->initWithFrame(client_rect, 0, 0);

	AppConfig *appConfig = AppConfig::Instance();
	std::string appid = appConfig->GetAppID();

	IWebPreferences *prefs = NULL;
	hr = CoCreateInstance(CLSID_WebPreferences, 0, CLSCTX_ALL,
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

			_bstr_t db_path(
					FileUtils::GetApplicationDataDirectory(appid).c_str());
			privatePrefs->setLocalStorageDatabasePath(db_path.copy());
			privatePrefs->Release();
		}

		web_view->setPreferences(prefs);
		prefs->Release();
	}

	// allow app:// and ti:// to run with local permissions (cross-domain ajax,etc)
	_bstr_t app_proto("app");
	web_view->registerURLSchemeAsLocal(app_proto.copy());

	_bstr_t ti_proto("ti");
	web_view->registerURLSchemeAsLocal(ti_proto.copy());

	IWebViewPrivate *web_view_private;
	hr = web_view->QueryInterface(IID_IWebViewPrivate,
			(void**) &web_view_private);
	hr = web_view_private->viewWindow((OLE_HANDLE*) &view_window_handle);

	hr = web_view_private->inspector(&web_inspector);
	if (FAILED(hr) || web_inspector == NULL)
	{
		logger->Error("Couldn't retrieve the web inspector object");
	}

	web_view_private->Release();

	_bstr_t inspector_url("ti://runtime/inspector/inspector.html");
	_bstr_t localized_strings_url("ti://runtime/inspector/localizedStrings.js");
	web_inspector->setInspectorURL(inspector_url.copy());
	web_inspector->setLocalizedStringsURL(localized_strings_url.copy());

	hr = web_view->mainFrame(&web_frame);
	//web_view->setShouldCloseWithWindow(TRUE);
}

Win32UserWindow::Win32UserWindow(WindowConfig* config, AutoUserWindow& parent) :
	UserWindow(config, parent),
	menu(0),
	activeMenu(0),
	nativeMenu(0),
	contextMenu(0),
	defaultIcon(0),
	web_inspector(0)
{
	logger = Logger::Get("UI.Win32UserWindow");
	
	win32_host = static_cast<kroll::Win32Host*>(binding->GetHost());
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
	
	//web_view = WebView::createInstance();
	logger->Debug("resize subviews");
	ResizeSubViews();

	// ensure we have valid restore values
	restore_bounds = GetBounds();
	restore_styles = GetWindowLong(window_handle, GWL_STYLE);

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
	this->requires_display = true;

	// set initial window icon to icon associated with exe file
	char exePath[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL), exePath, MAX_PATH);
	defaultIcon = ExtractIcon(win32_host->GetInstanceHandle(), exePath, 0);
	if (defaultIcon)
	{
		SendMessageA(window_handle, (UINT) WM_SETICON, ICON_BIG,
				(LPARAM) defaultIcon);
	}

	SetLayeredWindowAttributes(window_handle, 0, 255, LWA_ALPHA);
}

Win32UserWindow::~Win32UserWindow()
{
	if (web_view)
		web_view->Release();

	if (web_frame)
		web_frame->Release();
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
	GetClientRect(window_handle, &rcClient);
	MoveWindow(view_window_handle, 0, 0, rcClient.right, rcClient.bottom, TRUE);
}

HWND Win32UserWindow::GetWindowHandle()
{
	return this->window_handle;
}

void Win32UserWindow::Hide()
{
	ShowWindow(window_handle, SW_HIDE);
}

void Win32UserWindow::Show()
{
	ShowWindow(window_handle, SW_SHOW);
}

void Win32UserWindow::Minimize()
{
	ShowWindow(window_handle, SW_MINIMIZE);
}

void Win32UserWindow::Unminimize()
{
	ShowWindow(window_handle, SW_RESTORE);
}

bool Win32UserWindow::IsMinimized()
{
	return IsIconic(window_handle) != 0;
}

void Win32UserWindow::Maximize()
{
	ShowWindow(window_handle, SW_MAXIMIZE);
}

void Win32UserWindow::Unmaximize()
{
	ShowWindow(window_handle, SW_RESTORE);
}

bool Win32UserWindow::IsMaximized()
{
	return IsZoomed(window_handle) != 0;
}

void Win32UserWindow::Focus()
{
	SetFocus(window_handle);
}

void Win32UserWindow::Unfocus()
{
	//TODO: not sure exactly how to cause kill focus
}

void Win32UserWindow::Open()
{
	logger->Debug("Opening window_handle=%i, view_window_handle=%i", window_handle,  view_window_handle);

	UpdateWindow(window_handle);
	UpdateWindow(view_window_handle);

	ResizeSubViews();

	UserWindow::Open();
	SetURL(this->config->GetURL());
	if (!this->requires_display)
	{
		ShowWindow(window_handle, SW_SHOW);
		ShowWindow(view_window_handle, SW_SHOW);
	}
	
	this->SetupBounds();
	if (this->config->IsMaximized()) {
		this->Maximize();
	}
	else if (this->config->IsMinimized()) {
		this->Minimize();
	}
	
	FireEvent(OPENED);
}

void Win32UserWindow::Close()
{
	this->RemoveOldMenu();
	DestroyWindow(window_handle);
	UserWindow::Close();
	this->Closed();
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
	GetWindowRect(window_handle, &windowRect);
	GetClientRect(window_handle, &rect);

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
	
	if (this->config->IsUsingChrome()) {
		AdjustWindowRect(&boundsRect, GetWindowLong(window_handle, GWL_STYLE), !menu.isNull());
		this->chromeWidth = boundsRect.right - boundsRect.left - (int)bounds.width;
		this->chromeHeight = boundsRect.bottom - boundsRect.top - (int)bounds.height;
	}
	else {
		this->chromeWidth = 0;
		this->chromeHeight = 0;
	}
	
	SetWindowPos(window_handle, NULL, bounds.x, bounds.y, bounds.width + chromeWidth, bounds.height + chromeHeight, flags);
}

void Win32UserWindow::SetTitle(std::string& title)
{
	SetWindowText(window_handle, title.c_str());
}

void Win32UserWindow::SetURL(std::string& url_)
{
	std::string url = url_;
	url = ti::NormalizeAppURL(url);	
	logger->Debug("SetURL: %s", url.c_str());
	
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
	HRESULT hr = CoCreateInstance(CLSID_WebMutableURLRequest, 0, CLSCTX_ALL, IID_IWebMutableURLRequest, (void**)&request);
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
	hr = web_frame->loadRequest(request);
	if (FAILED(hr))
		goto exit;

	logger->Debug("set focus");
	SetFocus(view_window_handle);

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
	SetGWLFlag(window_handle, WS_SIZEBOX, this->config->IsUsingChrome() && resizable);
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
	return IsWindowVisible(window_handle);
}

void Win32UserWindow::SetTransparency(double transparency)
{
	if (config->GetTransparency() < 1.0)
	{
		SetWindowLong( this->window_handle, GWL_EXSTYLE, WS_EX_LAYERED);
		SetLayeredWindowAttributes(this->window_handle, 0, (BYTE) floor(
		config->GetTransparency() * 255), LWA_ALPHA);
	}
	else
	{
		SetWindowLong( this->window_handle, GWL_EXSTYLE, WS_EX_APPWINDOW);
	}
}

void Win32UserWindow::SetFullscreen(bool fullscreen)
{
	if (fullscreen)
	{
		restore_bounds = GetBounds();
		restore_styles = GetWindowLong(window_handle, GWL_STYLE);

		HMONITOR hmon = MonitorFromWindow(this->window_handle,
				MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		if (GetMonitorInfo(hmon, &mi))
		{
			SetWindowLong(window_handle, GWL_STYLE, 0);
			SetWindowPos(window_handle, NULL, 0, 0, mi.rcMonitor.right
					- mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_SHOWWINDOW);
		}

		FireEvent(FULLSCREENED);
	}
	else
	{
		SetWindowLong(window_handle, GWL_STYLE, restore_styles);
		SetBounds(restore_bounds);
		FireEvent(UNFULLSCREENED);
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
	SendMessageA(window_handle, (UINT) WM_SETICON, ICON_BIG, (LPARAM) icon);
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
	long windowStyle = GetWindowLong(this->window_handle, GWL_STYLE);

	SetFlag(windowStyle, WS_OVERLAPPED, config->IsUsingChrome());
	SetFlag(windowStyle, WS_CAPTION, config->IsUsingChrome());
	SetFlag(windowStyle, WS_SYSMENU, config->IsUsingChrome() && config->IsCloseable());
	SetFlag(windowStyle, WS_BORDER, config->IsUsingChrome());

	SetFlag(windowStyle, WS_MAXIMIZEBOX, config->IsMaximizable());
	SetFlag(windowStyle, WS_MINIMIZEBOX, config->IsMinimizable());

	SetWindowLong(this->window_handle, GWL_STYLE, windowStyle);

	if (showHide && config->IsVisible())
	{
		ShowWindow(window_handle, SW_HIDE);
		ShowWindow(window_handle, SW_SHOW);
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

	if (this->window_handle != NULL && this->nativeMenu) {
		::SetMenu(this->window_handle, NULL);
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

		if (!menu.isNull() && this->window_handle) {
			this->RemoveOldMenu();

			HMENU newNativeMenu = menu->CreateNativeTopLevel(true);
			::SetMenu(this->window_handle, newNativeMenu);
			this->nativeMenu = newNativeMenu;
		}
		this->activeMenu = menu;
	}
}

void Win32UserWindow::ReloadTiWindowConfig()
{
	//host->webview()->GetMainFrame()->SetAllowsScrolling(tiWindowConfig->isUsingScrollbars());
	//SetWindowText(hWnd, UTF8ToWide(tiWindowConfig->getTitle()).c_str());

	long windowStyle = GetWindowLong(this->window_handle, GWL_STYLE);

	SetFlag(windowStyle, WS_MINIMIZEBOX, config->IsMinimizable());
	SetFlag(windowStyle, WS_MAXIMIZEBOX, config->IsMaximizable());

	SetFlag(windowStyle, WS_OVERLAPPEDWINDOW, config->IsUsingChrome() && config->IsResizable());
	SetFlag(windowStyle, WS_CAPTION, config->IsUsingChrome());

	SetWindowLong(this->window_handle, GWL_STYLE, windowStyle);

	//UINT flags = SWP_NOZORDER | SWP_FRAMECHANGED;

	//SetLayeredWindowAttributes(hWnd, 0, (BYTE)0, LWA_ALPHA);
	if (config->GetTransparency() < 1.0)
	{
		SetWindowLong( this->window_handle, GWL_EXSTYLE, WS_EX_LAYERED);
		SetLayeredWindowAttributes(this->window_handle, 0, (BYTE) floor(
				config->GetTransparency() * 255), LWA_ALPHA);
	}
	else
	{
		SetWindowLong( this->window_handle, GWL_EXSTYLE, WS_EX_APPWINDOW);
	}
	
	SetLayeredWindowAttributes(this->window_handle, transparencyColor, 0,
			LWA_COLORKEY);
}

// called by frame load delegate to let the window know it's loaded
void Win32UserWindow::FrameLoaded()
{
	if (this->requires_display && this->config->IsVisible())
	{
		this->requires_display = false;
		ShowWindow(window_handle, SW_SHOW);
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
		SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE
				| SWP_NOSIZE);
	}
	else
	{
		SetWindowPos(window_handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE
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

void Win32UserWindow::ShowWebInspector()
{
	if (this->web_inspector)
	{
		this->web_inspector->show();
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

	SharedKList results = SelectFile(
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
	std::string filter;
	if (types.size() > 0)
	{
		//"All\0*.*\0Test\0*.TXT\0";
		filter.append(typesDescription);
		filter.push_back('\0');

		for (int i = 0; i < types.size(); i++)
		{
			std::string type = types.at(i);

			//multiple filters: "*.TXT;*.DOC;*.BAK"
			size_t found = type.find("*.");
			if (found != 0)
			{
				filter.append("*.");
			}
			filter.append(type);
			filter.append(";");
		}

		filter.push_back('\0');
	}

	OPENFILENAME ofn;
	char filen[MAX_PATH];
	ZeroMemory(&filen, sizeof(filen));
	if (defaultName.size() >= 0)
	{
		strcpy(filen, defaultName.c_str());
	}

	// init OPENFILE
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->window_handle;
	ofn.lpstrFile = filen;
	ofn.nMaxFile = sizeof(filen);
	ofn.lpstrFilter = (filter.empty() ? NULL : filter.c_str());
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = (path.length() == 0 ? NULL : path.c_str());
	ofn.Flags = OFN_EXPLORER;

	if (!title.empty())
	{
		ofn.lpstrTitle = title.c_str();
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
		result = GetSaveFileName(&ofn);
	}
	else
	{
		result = GetOpenFileName(&ofn);
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
		}
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
	bi.lpszTitle = title.c_str();
	bi.hwndOwner = this->window_handle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != 0)
	{
		TCHAR in_path[MAX_PATH];
		if (SHGetPathFromIDList(pidl, in_path))
		{
			results->Append(Value::NewString(std::string(in_path)));
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
	const char *s, std::vector<std::string> &tokens)
{
	std::string token;

	// input string is expected to be composed of single-NULL-separated tokens, and double-NULL terminated
	int i = 0;
	while (true)
	{
		char c;

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
	if (this->window_handle) {
		DrawMenuBar(this->window_handle);
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

