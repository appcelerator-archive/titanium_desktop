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
		HWND eventWindow = Win32Host::Win32Instance()->GetEventWindow();
		if (OpenClipboard(eventWindow))
		{
			EmptyClipboard();
			std::wstring wideString = UTF8ToWide(newText);
			HGLOBAL clipboardData = ::GlobalAlloc(GMEM_MOVEABLE, (newText.size()+1 * sizeof(char)));
			if (clipboardData == NULL)
			{
				CloseClipboard();
				return;
			}
			
			HGLOBAL wideClipboardData = ::GlobalAlloc(GMEM_MOVEABLE, (wideString.size()+1 * sizeof(wchar_t)));
			if (wideClipboardData == NULL)
			{
				CloseClipboard();
				return;
			}

			char *data = (char*) ::GlobalLock(clipboardData);
			strcpy(data, newText.c_str());
			::GlobalUnlock(clipboardData);
			SetClipboardData(CF_TEXT, clipboardData);
			
			wchar_t *wideData = (wchar_t*) ::GlobalLock(wideClipboardData);
			wcscpy(wideData, wideString.c_str());
			::GlobalUnlock(wideClipboardData);

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
			HWND eventWindow = Win32Host::Win32Instance()->GetEventWindow();
			if (OpenClipboard(eventWindow))
			{
				HANDLE clipboardData = GetClipboardData(CF_TEXT);
				if (clipboardData != NULL)
				{
					char *data = (char*) ::GlobalLock(clipboardData);
					if (data != NULL)
					{
						clipboardText.assign(data);
						::GlobalUnlock(clipboardData);
					}
				}
				else
				{
					HANDLE wideClipboardData = GetClipboardData(CF_UNICODETEXT);
					if (wideClipboardData != NULL)
					{
						wchar_t *data = (wchar_t*) ::GlobalLock(wideClipboardData);
						if (data != NULL)
						{
							clipboardText.assign(WideToUTF8(data));
							::GlobalUnlock(wideClipboardData);
						}
					}
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
		return IsClipboardFormatAvailable(CF_TEXT) != 0
			|| IsClipboardFormatAvailable(CF_UNICODETEXT) != 0;
	}

	void Clipboard::ClearTextImpl()
	{
		HWND eventWindow = Win32Host::Win32Instance()->GetEventWindow();
		if (OpenClipboard(eventWindow))
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
