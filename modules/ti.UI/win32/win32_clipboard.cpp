/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"

namespace ti
{
	Clipboard::~Clipboard()
	{
	}

	void Clipboard::SetTextImpl(std::string& newText)
	{
		if (newText.empty())
		{
			this->ClearText();
			return;
		}

		HWND eventWindow = Host::GetInstance()->GetEventWindow();
		if (!OpenClipboard(eventWindow))
		{
			std::string error(Win32Utils::QuickFormatMessage(GetLastError()));
			throw ValueException::FromFormat("Could not open Win32 clipbaord: %s",
				error.c_str());
		}

		// Do not empty clipboard here, because we want to preserve existing data of
		// different types.

		// We need to use the default ANSI code page when setting the clipboard
		// data, not the data passed in, which is in Unicode.
		std::wstring wideString(::UTF8ToWide(newText));
		std::string ansiString(::WideToMultiByte(wideString, CP_ACP));

		size_t length = ansiString.size() + 1;
		HGLOBAL clipboardData = ::GlobalAlloc(GMEM_MOVEABLE, length);
		if (!clipboardData)
		{
			CloseClipboard();
			std::string error(Win32Utils::QuickFormatMessage(GetLastError()));
			throw ValueException::FromFormat(
				"Could allocate ASNI clipbaord data: %s", error.c_str());
		}
		LPVOID data = ::GlobalLock(clipboardData);
		CopyMemory(data, ansiString.c_str(), length);
		::GlobalUnlock(clipboardData);
		SetClipboardData(CF_TEXT, clipboardData);

		length = (wideString.size() + 1) * sizeof(wchar_t);
		clipboardData = ::GlobalAlloc(GMEM_MOVEABLE, length);
		if (!clipboardData)
		{
			CloseClipboard();
			std::string error(Win32Utils::QuickFormatMessage(GetLastError()));
			throw ValueException::FromFormat(
				"Could allocate Unicode clipbaord data: %s", error.c_str());
		}
		data = ::GlobalLock(clipboardData);
		CopyMemory(data, wideString.c_str(), length);
		::GlobalUnlock(clipboardData);
		SetClipboardData(CF_UNICODETEXT, clipboardData);
		CloseClipboard();
	}

	std::string& Clipboard::GetTextImpl()
	{
		static std::string clipboardText;
		HWND eventWindow = Host::GetInstance()->GetEventWindow();
		if (!OpenClipboard(eventWindow))
		{
			std::string error(Win32Utils::QuickFormatMessage(GetLastError()));
			throw ValueException::FromFormat("Could not open Win32 clipbaord: %s",
				error.c_str());
		}

		// TODO(mrobinson): We need to also test for text/html here, I think.
		UINT format = 0;
		clipboardText = "";
		if (IsClipboardFormatAvailable(CF_UNICODETEXT))
			format = CF_UNICODETEXT;
		else if (IsClipboardFormatAvailable(CF_TEXT))
			format = CF_TEXT;
		else
			return clipboardText;

		HANDLE clipboardData = GetClipboardData(format);
		if (!clipboardData)
		{
			CloseClipboard();
			std::string error(Win32Utils::QuickFormatMessage(GetLastError()));
			throw ValueException::FromFormat(
				"Could not get clipbaord data: %s", error.c_str());
		}

		LPVOID data = ::GlobalLock(clipboardData);
		if (!data)
		{
			CloseClipboard();
			std::string error(Win32Utils::QuickFormatMessage(GetLastError()));
			throw ValueException::FromFormat(
				"Could not lock clipbaord data: %s", error.c_str());
		}

		if (format == CF_TEXT)
		{
			std::string ansiText(static_cast<char*>(data));
			clipboardText = ::MultiByteToMultiByte(ansiText, CP_ACP, CP_UTF8);
		}
		else if (format == CF_UNICODETEXT)
		{
			std::wstring wideString(static_cast<wchar_t*>(data));
			clipboardText = ::WideToUTF8(wideString);
		}

		::GlobalUnlock(clipboardData);
		CloseClipboard();
		return clipboardText;
	}

	bool Clipboard::HasTextImpl()
	{
		return IsClipboardFormatAvailable(CF_TEXT) != 0
			|| IsClipboardFormatAvailable(CF_UNICODETEXT) != 0;
	}

	void Clipboard::ClearTextImpl()
	{
		HWND eventWindow = Host::GetInstance()->GetEventWindow();

		// TODO(mrobinson): If possible we need to clear only the text portion here.
		if (!OpenClipboard(eventWindow))
		{
			std::string error(Win32Utils::QuickFormatMessage(GetLastError()));
			throw ValueException::FromFormat("Could not open Win32 clipbaord: %s",
				error.c_str());
		}

		EmptyClipboard();
		CloseClipboard();
	}

	BytesRef Clipboard::GetImageImpl(std::string& mimeType)
	{
		BytesRef image(0);
		return image;
	}

	void Clipboard::SetImageImpl(std::string& mimeType, BytesRef image)
	{
	}

	bool Clipboard::HasImageImpl()
	{
		return false;
	}

	void Clipboard::ClearImageImpl()
	{
	}

	std::vector<std::string>& Clipboard::GetURIListImpl()
	{
		static std::vector<std::string> uriList;
		uriList.clear();
		return uriList;
	}

	void Clipboard::SetURIListImpl(std::vector<std::string>& uriList)
	{
	}

	bool Clipboard::HasURIListImpl()
	{
		return false;
	}

	void Clipboard::ClearURIListImpl()
	{
	}
}
