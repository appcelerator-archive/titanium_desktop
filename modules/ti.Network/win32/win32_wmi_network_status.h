/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef WIN32_WMI_NETWORK_STATUS_H_
#define WIN32_WMI_NETWORK_STATUS_H_

#include <kroll/base.h>
#include <windows.h>
#include <wbemidl.h>
#include <kroll/kroll.h>

namespace ti
{
	class Win32WMINetworkStatus : public IWbemObjectSink
	{
	protected:
		KMethodRef *callback;
		int ref_count;

	public:
		Win32WMINetworkStatus(KMethodRef callback);

		//// IWbemObjectSink
		virtual HRESULT STDMETHODCALLTYPE Indicate(long lObjectCount, IWbemClassObject** apObjArray );
		virtual HRESULT STDMETHODCALLTYPE SetStatus(long lFlags, HRESULT hResult, BSTR strParam, IWbemClassObject* pObjectParam);

		//// IUnknown
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
	};
}
#endif
