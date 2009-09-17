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
		if (OpenClipboard(NULL))
		{
			EmptyClipboard();
			std::wstring wideString = UTF8ToWide(newText);
			HGLOBAL clipboardData = ::GlobalAlloc(GMEM_DDESHARE, newText.size()+1);
			HGLOBAL wideClipboardData = ::GlobalAlloc(GMEM_DDESHARE, wideString.size()+1);

			char *data = (char*) ::GlobalLock(clipboardData);
			wchar_t *wideData = (wchar_t*) ::GlobalLock(wideClipboardData);

			strcpy(data, newText.c_str());
			wcscpy(wideData, wideString.c_str());

			::GlobalUnlock(clipboardData);
			::GlobalUnlock(wideClipboardData);

			SetClipboardData(CF_TEXT, clipboardData);
			SetClipboardData(CF_UNICODETEXT, wideClipboardData);
			CloseClipboard();
		}
		else
		{
			throw ValueException::FromString("Couldn't open clipboard in setText");
		}
	}

	std::string& Clipboard::GetTextImpl()
	{
		static std::string clipboardText;
		if (HasText())
		{
			if (OpenClipboard(NULL))
			{
				HANDLE clipboardData = GetClipboardData(CF_TEXT);
				HANDLE wideClipboardData = GetClipboardData(CF_UNICODETEXT);
				if (clipboardData != NULL)
				{
					char *data = (char*) ::GlobalLock(clipboardData);
					clipboardText.assign(data);
					::GlobalUnlock(clipboardData);
				}
				else if (wideClipboardData != NULL)
				{
					wchar_t *data = (wchar_t*) ::GlobalLock(wideClipboardData);
					clipboardText.assign(WideToUTF8(data));
					::GlobalUnlock(wideClipboardData);
				}
				CloseClipboard();
			}
			else
			{
				throw ValueException::FromString("Couldn't open clipboard in getText");
			}
		}
		else
		{
			clipboardText = "";
		}

		return clipboardText;
	}

	bool Clipboard::HasTextImpl()
	{
		if (OpenClipboard(NULL))
		{
			bool hasText = IsClipboardFormatAvailable(CF_TEXT) != 0
				|| IsClipboardFormatAvailable(CF_UNICODETEXT) != 0;

			CloseClipboard();
			return hasText;
		}
		else
		{
			throw ValueException::FromString("Couldn't open clipboard in hasText");
		}
		return false;
	}

	void Clipboard::ClearTextImpl()
	{
		if (OpenClipboard(NULL))
		{
			EmptyClipboard();
			CloseClipboard();
		}
		else
		{
			throw ValueException::FromString("Couldn't open clipboard in clearData");
		}
	}

	AutoBlob Clipboard::GetImageImpl(std::string& mimeType)
	{
		AutoBlob image(0);
		return image;
	}

	void Clipboard::SetImageImpl(std::string& mimeType, AutoBlob image)
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
