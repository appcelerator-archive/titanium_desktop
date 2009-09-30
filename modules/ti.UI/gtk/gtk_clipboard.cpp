/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"

namespace ti
{
	static void GetClipboardData(GtkClipboard* clipboard, GtkSelectionData* selectionData,
		guint info, gpointer data);
	static void ClearClipboardData(GtkClipboard* clipboard, gpointer data);
	static void OwnerChangeCallback(GtkClipboard*, GdkEvent*, gpointer);

	static inline GtkClipboard* GetClipboard()
	{
		static GtkClipboard* clipboard = NULL;

		if (!clipboard)
		{
			clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
			g_signal_connect(
				G_OBJECT(clipboard), "owner-change",
				G_CALLBACK(OwnerChangeCallback), NULL);
		}
		return clipboard;
	}

	struct ClipboardPrivate
	{
		gchar* text;
		gchar** uris;
		bool preserve;
		bool ownClipboard;

		enum ClipboardDataIds { TEXT_DATA, URI_LIST_DATA, IMAGE_DATA };
		inline void SetText(std::string& newText)
		{
			this->ClearText();
			this->text = g_strdup(newText.c_str());
		}

		inline void SetURIList(std::vector<std::string>& newURIList)
		{
			int size = newURIList.size() + 1;
			this->ClearURIList();
			this->uris = g_new0(gchar*, size);

			for (int i = 0; i < size - 1; i++)
				this->uris[i] = g_strdup(newURIList[i].c_str());
		}

		inline void ClearText()
		{
			g_free(this->text);
			this->text = NULL;
		}

		inline void ClearURIList()
		{
			g_strfreev(this->uris);
			this->uris = NULL;
		}

		inline void ClearImage()
		{
			// TODO: Clear image data here.
		}

		void SetupClipboardContentTypes()
		{
			GtkTargetList* list = gtk_target_list_new(NULL, 0);

			// TODO: Support for images
			if (!this->text && !this->uris)
				return;

			if (this->text)
				gtk_target_list_add_text_targets(list, ClipboardPrivate::TEXT_DATA);

			if (this->uris)
				gtk_target_list_add_uri_targets(list, ClipboardPrivate::URI_LIST_DATA);

			int size = 0;
			GtkTargetEntry* table = gtk_target_table_new_from_list(list, &size);
			if (table)
			{
				// gtk_clipboard_set_with_data may try to clear our clipboard when we
				// call it, so we turn on a flag here which prevents our clipboard data
				// from being freed during this call.
				this->preserve = true;
				GtkClipboard* clipboard = GetClipboard();
				if (gtk_clipboard_set_with_data(clipboard, table, size, GetClipboardData,
					ClearClipboardData, NULL))
				{
					this->ownClipboard = true;
					gtk_clipboard_set_can_store(clipboard, NULL, 0);
				}
				this->preserve = false;

				gtk_target_table_free(table, size);
			}

			gtk_target_list_unref(list);
		}

	} priv;

	static void GetClipboardData(GtkClipboard* clipboard, GtkSelectionData* selectionData,
		guint info, gpointer data)
	{
		if (info == ClipboardPrivate::TEXT_DATA && priv.text)
		{
			gtk_selection_data_set_text(selectionData, priv.text, -1);
		}

		else if (info == ClipboardPrivate::URI_LIST_DATA && priv.uris)
			gtk_selection_data_set_uris(selectionData, priv.uris);

		// TODO: Image data
	}

	static void ClearClipboardData(GtkClipboard* clipboard, gpointer data)
	{
		if (priv.preserve)
			return;

		priv.ClearText();
		priv.ClearURIList();
		priv.ClearImage();
	}

	static void OwnerChangeCallback(GtkClipboard*, GdkEvent*, gpointer)
	{
		priv.ownClipboard = false;
	}

	Clipboard::~Clipboard()
	{
	}

	void Clipboard::SetTextImpl(std::string& newText)
	{
		priv.SetText(newText);
		priv.SetupClipboardContentTypes();
	}

	std::string& Clipboard::GetTextImpl()
	{
		static std::string clipboardText;

		if (this->HasText())
		{
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
		return (priv.ownClipboard && priv.text) ||
			(!priv.ownClipboard && gtk_clipboard_wait_is_text_available(GetClipboard()));
	}

	void Clipboard::ClearTextImpl()
	{
		priv.ClearText();
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

		gchar** uris = gtk_clipboard_wait_for_uris(GetClipboard());
		if (!uris)
			return uriList;

		gchar** current = uris;
		while (*current)
			uriList.push_back(*current++);

		g_strfreev(uris);

		return uriList;
	}

	void Clipboard::SetURIListImpl(std::vector<std::string>& uriList)
	{
		priv.SetURIList(uriList);
		priv.SetupClipboardContentTypes();
	}

	bool Clipboard::HasURIListImpl()
	{
		return (priv.ownClipboard && priv.uris) ||
			(!priv.ownClipboard && gtk_clipboard_wait_is_uris_available(GetClipboard()));
	}

	void Clipboard::ClearURIListImpl()
	{
		priv.ClearURIList();
		priv.SetupClipboardContentTypes();
	}
}
