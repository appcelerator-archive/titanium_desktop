/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"

namespace ti
{
	static inline GtkClipboard* GetClipboard()
	{
		static GtkClipboard* clipboard = NULL;

		if (!clipboard)
		{
			clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
		}
		return clipboard;
	}

	Clipboard::~Clipboard()
	{
	}

	void Clipboard::SetTextImpl(std::string& newText)
	{
		gtk_clipboard_set_text(GetClipboard(), newText.c_str(), -1);
	}

	std::string& Clipboard::GetTextImpl()
	{
		static std::string clipboardText;

		if (this->HasText())
		{
			// TODO: It might make sense to do this asynchronously.
			gchar* clipboardChars = gtk_clipboard_wait_for_text(GetClipboard());
			if (!clipboardChars)
			{
				clipboardText = "";
				throw ValueException::FromString("Could not retrieve clipboard data");
			}

			clipboardText = clipboardChars;
			g_free(clipboardChars);
		}
		else
		{
			clipboardText = "";
		}

		return clipboardText;
	}

	bool Clipboard::HasTextImpl()
	{
		return gtk_clipboard_wait_is_text_available(GetClipboard());
	}

	void Clipboard::ClearTextImpl()
	{
		gtk_clipboard_clear(GetClipboard());
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
