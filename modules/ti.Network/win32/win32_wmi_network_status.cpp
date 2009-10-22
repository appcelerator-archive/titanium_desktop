/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "win32_wmi_network_status.h"

#include <comutil.h>

namespace ti
{
	Win32WMINetworkStatus::Win32WMINetworkStatus(KMethodRef callback) : ref_count(1)
	{
		/*this->callback = new KMethodRef(callback);

		IWbemLocator* locator;
		HRESULT hr = CoCreateInstance( CLSID_WbemAdministrativeLocator, NULL,
		                               CLSCTX_INPROC_SERVER, IID_IWbemLocator,
		                               (void**)&locator);

		IWbemServices* service;
		hr = locator->ConnectServer( L"winmgmts:root\\wmi", NULL, NULL, NULL,
		                             WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &service );

		hr = service->ExecNotificationQueryAsync(L"WQL", L"SELECT * FROM MSNdis_StatusMediaConnect", 0, 0, this);
		hr = service->ExecNotificationQueryAsync(L"WQL", L"SELECT * FROM MSNdis_StatusMediaDisconnect", 0, 0, this);*/
	}

	//// IWbemObjectSink
	HRESULT STDMETHODCALLTYPE
	Win32WMINetworkStatus::Indicate(long lObjectCount, IWbemClassObject** apObjArray )
	{
		std::cout << "Indicate!" << std::endl;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE
	Win32WMINetworkStatus::SetStatus(long lFlags, HRESULT hResult, BSTR strParam, IWbemClassObject* pObjectParam )
	{
		std::string str = _bstr_t(strParam);
		std::cout << "WMI set status: " << str << std::endl;

		return S_OK;
	}


	//// IUnknown
	HRESULT STDMETHODCALLTYPE
	Win32WMINetworkStatus::QueryInterface(REFIID iid, void ** ppvObject)
	{
		*ppvObject = 0;
		if (IsEqualGUID(iid, IID_IUnknown)) {
			*ppvObject = static_cast<IUnknown*>(this);
		}
		else if (IsEqualGUID(iid, IID_IWbemObjectSink)) {
			*ppvObject = static_cast<IWbemObjectSink*>(this);
		}
		else {
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE
	Win32WMINetworkStatus::AddRef()
	{
		return ++ref_count;
	}

	ULONG STDMETHODCALLTYPE
	Win32WMINetworkStatus::Release()
	{
		ULONG new_count = --ref_count;
		if (!new_count) delete(this);

		return new_count;
	}
}
