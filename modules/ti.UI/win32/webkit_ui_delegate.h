/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef TI_WEBKIT_WIN32_UI_DELEGATE_H_
#define TI_WEBKIT_WIN32_UI_DELEGATE_H_
#include <windows.h>
namespace ti
{

class Win32UserWindow;
class Win32WebKitUIDelegate : public IWebUIDelegate2, IWebUIDelegatePrivate
{
public:
	Win32WebKitUIDelegate(Win32UserWindow *window);

	// These function declarations copied from WebKit DumpRenderTree (UIDelegate.cpp)
	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	// IWebUIDelegate
	virtual HRESULT STDMETHODCALLTYPE createWebViewWithRequest(
		/* [in] */ IWebView *sender,
		/* [in] */ IWebURLRequest *request,
		/* [retval][out] */ IWebView **newWebView)
	{
		return createWebViewWithRequest(sender, request, 0, newWebView);
	}

	virtual HRESULT STDMETHODCALLTYPE webViewShow(
		/* [in] */ IWebView *sender) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewClose(
		/* [in] */ IWebView *sender);

	virtual HRESULT STDMETHODCALLTYPE webViewFocus(
		/* [in] */ IWebView *sender);

	virtual HRESULT STDMETHODCALLTYPE webViewUnfocus(
		/* [in] */ IWebView *sender);

	virtual HRESULT STDMETHODCALLTYPE webViewFirstResponder(
		/* [in] */ IWebView *sender,
		/* [retval][out] */ OLE_HANDLE *responder) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE makeFirstResponder(
		/* [in] */ IWebView *sender,
		/* [in] */ OLE_HANDLE responder) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE setStatusText(
		/* [in] */ IWebView *sender,
		/* [in] */ BSTR text);

	virtual HRESULT STDMETHODCALLTYPE webViewStatusText(
		/* [in] */ IWebView *sender,
		/* [retval][out] */ BSTR *text) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewAreToolbarsVisible(
		/* [in] */ IWebView *sender,
		/* [retval][out] */ BOOL *visible) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE setToolbarsVisible(
		/* [in] */ IWebView *sender,
		/* [in] */ BOOL visible) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewIsStatusBarVisible(
		/* [in] */ IWebView *sender,
		/* [retval][out] */ BOOL *visible) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE setStatusBarVisible(
		/* [in] */ IWebView *sender,
		/* [in] */ BOOL visible) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewIsResizable(
		/* [in] */ IWebView *sender,
		/* [retval][out] */ BOOL *resizable) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE setResizable(
		/* [in] */ IWebView *sender,
		/* [in] */ BOOL resizable) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE setFrame(
		/* [in] */ IWebView *sender,
		/* [in] */ RECT *frame);

	virtual HRESULT STDMETHODCALLTYPE webViewFrame(
		/* [in] */ IWebView *sender,
		/* [retval][out] */ RECT *frame);

	virtual HRESULT STDMETHODCALLTYPE setContentRect(
		/* [in] */ IWebView *sender,
		/* [in] */ RECT *contentRect) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewContentRect(
		/* [in] */ IWebView *sender,
		/* [retval][out] */ RECT *contentRect) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE runJavaScriptAlertPanelWithMessage(
		/* [in] */ IWebView *sender,
		/* [in] */ BSTR message);

	virtual HRESULT STDMETHODCALLTYPE runJavaScriptConfirmPanelWithMessage(
		/* [in] */ IWebView *sender,
		/* [in] */ BSTR message,
		/* [retval][out] */ BOOL *result);

	virtual HRESULT STDMETHODCALLTYPE runJavaScriptTextInputPanelWithPrompt(
		/* [in] */ IWebView *sender,
		/* [in] */ BSTR message,
		/* [in] */ BSTR defaultText,
		/* [retval][out] */ BSTR *result);

	virtual HRESULT STDMETHODCALLTYPE runBeforeUnloadConfirmPanelWithMessage(
		/* [in] */ IWebView *sender,
		/* [in] */ BSTR message,
		/* [in] */ IWebFrame *initiatedByFrame,
		/* [retval][out] */ BOOL *result);

	virtual HRESULT STDMETHODCALLTYPE runOpenPanelForFileButtonWithResultListener(
		/* [in] */ IWebView *sender,
		/* [in] */ IWebOpenPanelResultListener *resultListener) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE mouseDidMoveOverElement(
		/* [in] */ IWebView *sender,
		/* [in] */ IPropertyBag *elementInformation,
		/* [in] */ UINT modifierFlags) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE contextMenuItemsForElement(
		/* [in] */ IWebView *sender,
		/* [in] */ IPropertyBag *element,
		/* [in] */ OLE_HANDLE defaultItems,
		/* [retval][out] */ OLE_HANDLE *resultMenu) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE validateUserInterfaceItem(
		/* [in] */ IWebView *webView,
		/* [in] */ UINT itemCommandID,
		/* [in] */ BOOL defaultValidation,
		/* [retval][out] */ BOOL *isValid) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE shouldPerformAction(
		/* [in] */ IWebView *webView,
		/* [in] */ UINT itemCommandID,
		/* [in] */ UINT sender) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE dragDestinationActionMaskForDraggingInfo(
		/* [in] */ IWebView *webView,
		/* [in] */ IDataObject *draggingInfo,
		/* [retval][out] */ WebDragDestinationAction *action) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE willPerformDragDestinationAction(
		/* [in] */ IWebView *webView,
		/* [in] */ WebDragDestinationAction action,
		/* [in] */ IDataObject *draggingInfo) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE dragSourceActionMaskForPoint(
		/* [in] */ IWebView *webView,
		/* [in] */ LPPOINT point,
		/* [retval][out] */ WebDragSourceAction *action) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE willPerformDragSourceAction(
		/* [in] */ IWebView *webView,
		/* [in] */ WebDragSourceAction action,
		/* [in] */ LPPOINT point,
		/* [in] */ IDataObject *pasteboard,
		/* [retval][out] */ IDataObject **newPasteboard) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE contextMenuItemSelected(
		/* [in] */ IWebView *sender,
		/* [in] */ void *item,
		/* [in] */ IPropertyBag *element) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE hasCustomMenuImplementation(
		/* [retval][out] */ BOOL *hasCustomMenus);

	virtual HRESULT STDMETHODCALLTYPE trackCustomPopupMenu(
		/* [in] */ IWebView *sender,
		/* [in] */ OLE_HANDLE menu,
		/* [in] */ LPPOINT point);

	virtual HRESULT STDMETHODCALLTYPE measureCustomMenuItem(
		/* [in] */ IWebView *sender,
		/* [in] */ void *measureItem) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE drawCustomMenuItem(
		/* [in] */ IWebView *sender,
		/* [in] */ void *drawItem) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE addCustomMenuDrawingData(
		/* [in] */ IWebView *sender,
		/* [in] */ OLE_HANDLE menu) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE cleanUpCustomMenuDrawingData(
		/* [in] */ IWebView *sender,
		/* [in] */ OLE_HANDLE menu) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE canTakeFocus(
		/* [in] */ IWebView *sender,
		/* [in] */ BOOL forward,
		/* [out] */ BOOL *result) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE takeFocus(
		/* [in] */ IWebView *sender,
		/* [in] */ BOOL forward) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE registerUndoWithTarget(
		/* [in] */ IWebUndoTarget *target,
		/* [in] */ BSTR actionName,
		/* [in] */ IUnknown *actionArg);

	virtual HRESULT STDMETHODCALLTYPE removeAllActionsWithTarget(
		/* [in] */ IWebUndoTarget *target);

	virtual HRESULT STDMETHODCALLTYPE setActionTitle(
		/* [in] */ BSTR actionTitle);

	virtual HRESULT STDMETHODCALLTYPE undo();

	virtual HRESULT STDMETHODCALLTYPE redo();

	virtual HRESULT STDMETHODCALLTYPE canUndo(
		/* [retval][out] */ BOOL *result);

	virtual HRESULT STDMETHODCALLTYPE canRedo(
		/* [retval][out] */ BOOL *result);

	virtual HRESULT STDMETHODCALLTYPE printFrame(
		/* [in] */ IWebView *webView,
		/* [in] */ IWebFrame *frame) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE ftpDirectoryTemplatePath(
		/* [in] */ IWebView *webView,
		/* [retval][out] */ BSTR *path) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewHeaderHeight(
		/* [in] */ IWebView *webView,
		/* [retval][out] */ float *result) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewFooterHeight(
		/* [in] */ IWebView *webView,
		/* [retval][out] */ float *result) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE drawHeaderInRect(
		/* [in] */ IWebView *webView,
		/* [in] */ RECT *rect,
		/* [in] */ OLE_HANDLE drawingContext) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE drawFooterInRect(
		/* [in] */ IWebView *webView,
		/* [in] */ RECT *rect,
		/* [in] */ OLE_HANDLE drawingContext,
		/* [in] */ UINT pageIndex,
		/* [in] */ UINT pageCount) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewPrintingMarginRect(
		/* [in] */ IWebView *webView,
		/* [retval][out] */ RECT *rect) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE canRunModal(
		/* [in] */ IWebView *webView,
		/* [retval][out] */ BOOL *canRunBoolean) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE createModalDialog(
		/* [in] */ IWebView *sender,
		/* [in] */ IWebURLRequest *request,
		/* [retval][out] */ IWebView **newWebView) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE runModal(
		/* [in] */ IWebView *webView) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE isMenuBarVisible(
		/* [in] */ IWebView *webView,
		/* [retval][out] */ BOOL *visible) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE setMenuBarVisible(
		/* [in] */ IWebView *webView,
		/* [in] */ BOOL visible) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE runDatabaseSizeLimitPrompt(
		/* [in] */ IWebView *webView,
		/* [in] */ BSTR displayName,
		/* [in] */ IWebFrame *initiatedByFrame,
		/* [retval][out] */ BOOL *allowed) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE paintCustomScrollbar(
		/* [in] */ IWebView *webView,
		/* [in] */ HDC hDC,
		/* [in] */ RECT rect,
		/* [in] */ WebScrollBarControlSize size,
		/* [in] */ WebScrollbarControlState state,
		/* [in] */ WebScrollbarControlPart pressedPart,
		/* [in] */ BOOL vertical,
		/* [in] */ float value,
		/* [in] */ float proportion,
		/* [in] */ WebScrollbarControlPartMask parts) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE paintCustomScrollCorner(
		/* [in] */ IWebView *webView,
		/* [in] */ HDC hDC,
		/* [in] */ RECT rect) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE newBackingStore(
		/* [in] */ IWebView *webView,
		/* [in] */ OLE_HANDLE bitmap);

protected:
	// IWebUIDelegatePrivate
	virtual HRESULT STDMETHODCALLTYPE createWebViewWithRequest(
		/* [in] */ IWebView *sender,
		/* [in] */ IWebURLRequest *request,
		/* [in] */ IPropertyBag *features,
		/* [retval][out] */ IWebView **newWebView);

	virtual HRESULT STDMETHODCALLTYPE unused1() { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE unused2() { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE unused3() { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewScrolled(
		/* [in] */ IWebView *sender) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewAddMessageToConsole(
		/* [in] */ IWebView *sender,
		/* [in] */ BSTR message,
		/* [in] */ int lineNumber,
		/* [in] */ BSTR url,
		/* [in] */ BOOL isError);

	virtual HRESULT STDMETHODCALLTYPE webViewShouldInterruptJavaScript(
		/* [in] */ IWebView *sender,
		/* [retval][out] */ BOOL *result) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewReceivedFocus(
		/* [in] */ IWebView *sender);

	virtual HRESULT STDMETHODCALLTYPE webViewLostFocus(
		/* [in] */ IWebView *sender,
		/* [in] */ OLE_HANDLE loseFocusTo);

	virtual HRESULT STDMETHODCALLTYPE doDragDrop(
		/* [in] */ IWebView *sender,
		/* [in] */ IDataObject *dataObject,
		/* [in] */ IDropSource *dropSource,
		/* [in] */ DWORD okEffect,
		/* [retval][out] */ DWORD *performedEffect);

	virtual HRESULT STDMETHODCALLTYPE webViewGetDlgCode(
		/* [in] */ IWebView *sender,
		/* [in] */ UINT keyCode,
		/* [retval][out] */ LONG_PTR *code);

	virtual HRESULT STDMETHODCALLTYPE webViewPainted(
		/* [in] */ IWebView *sender);

	virtual HRESULT STDMETHODCALLTYPE exceededDatabaseQuota(
		/* [in] */ IWebView *sender,
		/* [in] */ IWebFrame *frame,
		/* [in] */ IWebSecurityOrigin *origin,
		/* [in] */ BSTR databaseIdentifier);

	virtual HRESULT STDMETHODCALLTYPE embeddedViewWithArguments(
		/* [in] */ IWebView *sender,
		/* [in] */ IWebFrame *frame,
		/* [in] */ IPropertyBag *arguments,
		/* [retval][out] */ IWebEmbeddedView **view) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewClosing(
		/* [in] */ IWebView *sender) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewSetCursor(
		/* [in] */ IWebView *sender,
		/* [in] */ OLE_HANDLE cursor) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE webViewDidInvalidate(
		/* [in] */ IWebView *sender) { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE desktopNotificationsDelegate(
		/* [out] */ IWebDesktopNotificationsDelegate** result) { return E_NOTIMPL; }

private:
	Win32UserWindow* window;
	HMENU nativeContextMenu;
	Logger* logger;
	int referenceCount;
};

} // namespace ti

#endif

