/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009, 2010 Appcelerator, Inc. All Rights Reserved.
 */

#include "../network_status.h"
#include <comutil.h>
#include <wbemidl.h>
#include <Poco/Net/DNS.h>
#include <Poco/Net/HostEntry.h>
#include <Poco/Net/IPAddress.h>

namespace ti
{

static void LogError(const std::string& message)
{
	static Logger* logger = Logger::Get("Network.NetworkStatus");
	logger->Error(message);
}

static void HandleHResultError(std::string message, HRESULT result)
{
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
	LogError(message);
}

static IWbemLocator* locator = 0;
static IWbemServices* service = 0;

void NetworkStatus::InitializeLoop()
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	HRESULT result = CoInitializeSecurity(0, -1, 0, 0, RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE, 0, EOAC_NONE, 0 );
	if (FAILED(result))
	{
		HandleHResultError("Failed to initialize COM security", result);
		return;
	}

	result = CoCreateInstance(CLSID_WbemAdministrativeLocator, 0,
		CLSCTX_INPROC_SERVER, IID_IWbemLocator,
		reinterpret_cast<void**>(&locator));
	if (FAILED(result))
	{
		HandleHResultError("Failed to initialize NetworkStatus locator", result);
		return;
	}

	result = locator->ConnectServer(L"root\\cimv2", 0, 0, 0,
		WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &service);
	if (FAILED(result))
	{
		HandleHResultError("Failed to initialize NetworkStatus service", result);
		return;
	}
}

void NetworkStatus::CleanupLoop()
{
	service->Release();
	service = 0;

	locator->Release();
	locator = 0;

	CoUninitialize();
}

static bool TryAddress(const std::string& ipAddress)
{
	IEnumWbemClassObject* enumerator = 0;

	std::wstring query(L"SELECT * FROM Win32_PingStatus WHERE (Address=\"");
	query.append(::UTF8ToWide(ipAddress));
	query.append(L"\")");
	BSTR queryBstr = SysAllocString(query.c_str());

	HRESULT result = service->ExecQuery(L"WQL", queryBstr,
		WBEM_FLAG_FORWARD_ONLY, 0, &enumerator);
	SysFreeString(queryBstr);

	if (FAILED(result))
	{
		HandleHResultError("Failed to get lauch query", result);
		enumerator->Release();
		return true;
	}

	ULONG count;
	IWbemClassObject* ping = 0;
	result = enumerator->Next(WBEM_INFINITE, 1L, &ping, &count);
	if (FAILED(result))
	{
		HandleHResultError("Failed to get ping data", result);
		enumerator->Release();
		return true;
	}

	if (count < 1)
	{
		LogError("Did not find ping result");
		enumerator->Release();
		ping->Release();
		return true;
	}

	variant_t pingValue;
	result = ping->Get(L"StatusCode", 0, &pingValue, 0, 0);
	if (FAILED(result))
	{
		HandleHResultError("Could not get ping StatusCode value", result);
		enumerator->Release();
		ping->Release();
		return true;
	}

	ping->Release();
	return static_cast<int>(pingValue) == 0;;
}

static bool TryHost(const std::string& host)
{
	// WMI waits a long time when the network connection goes down
	// and it cannot resolve DNS, so have Poco do the resolution
	// and then ping using WMI.
	Poco::Net::HostEntry entry;
	try
	{
		entry = Poco::Net::DNS::resolve(host);
	}
	catch (...)
	{
		return false;
	}

	const Poco::Net::HostEntry::AddressList& addresses = entry.addresses();
	for (size_t i = 0; i < addresses.size(); i++)
	{
		const Poco::Net::IPAddress& address = addresses[i];
		if (address.isIPv4Compatible() && TryAddress(address.toString()))
			return true;
	}

	return false;
}

bool NetworkStatus::GetStatus()
{
	if (!locator || !service)
		return true;

	if (TryHost("google.com"))
		return true;

	if (TryHost("yahoo.com"))
		return true;

	return false;
}

}
