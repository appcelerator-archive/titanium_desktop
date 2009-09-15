/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "ui_module.h"

namespace ti
{
	Clipboard::Clipboard() :
		AccessorBoundObject("Clipboard")
	{
		/**
		 * @tiapi(method=True,name=UI.Clipboard.setText,since=0.7)
		 * @tiapi Set the data on the clipboard given a mime-type and the new data.
		 * @tiapi This method will set data on the appropriate portion of the clipboard
		 * @tiapi for the given mime-type.
		 * @tiarg[String, type] The mime-type of the data to set.
		 * @tiarg[String, data] The new clipboard text.
		 */
		this->SetMethod("setData", &Clipboard::_SetData);

		/**
		 * @tiapi(method=True,name=UI.Clipboard.getText,since=0.7)
		 * @tiapi Get the data on the clipboard from the portion which contains
		 * @tiapi data of the given mime-type.
		 * @tiarg[String, type] The mime-type of the data to get.
		 * @tiresult[String] The data from the appropriate portion of the clipboard.
		 */
		this->SetMethod("getData", &Clipboard::_GetData);

		/**
		 * @tiapi(method=True,name=UI.Clipboard.clearData,since=0.7)
		 * @tiapi Clear data of the given mime-type from  the clipboard. If no
		 * @tiapi mime-type is given, clear all data from the clipboard.
		 * @tiarg[String, type, optional=True] The mime-type of the data to clear.
		 */
		this->SetMethod("clearData", &Clipboard::_ClearData);

		/**
		 * @tiapi(method=True,name=UI.Clipboard.setText,since=0.7)
		 * @tiapi Set the text on the clipboard. This will overwrite
		 * @tiapi the current contents of the clipboard.
		 * @tiarg[String, newText] The new clipboard text.
		 */
		this->SetMethod("setText", &Clipboard::_SetText);

		/**
		 * @tiapi(method=True,name=UI.Clipboard.getText,since=0.7)
		 * @tiapi Get the current text on the clipboard. 
		 * @tiresult[String] The current text on the clipboard or an empty
		 * @tiresult string if the text portion of the clipboard is empty.
		 */
		this->SetMethod("getText", &Clipboard::_GetText);

		/**
		 * @tiapi(method=True,name=UI.Clipboard.clearData,since=0.7)
		 * @tiapi Clear the text portion of the clipboard.
		 */
		this->SetMethod("clearText", &Clipboard::_ClearText);

		/**
		 * @tiapi(method=True,name=UI.Clipboard.hasText,since=0.7)
		 * @tiapi Return true if there is any content in the text portion of the clipboard.
		 * @tiresult[Boolean] True if there is text on the clipboard or false otherwise.
		 */
		this->SetMethod("hasText", &Clipboard::_HasText);
	}

	void Clipboard::_GetData(const ValueList& args, SharedValue result)
	{
		// TODO: Support the mime-type parameter.
		result->SetString(this->GetText());
	}

	void Clipboard::_SetData(const ValueList& args, SharedValue result)
	{
		// TODO: Support the mime-type parameter.
		args.VerifyException("setData", "s s");
		std::string newText(args.GetString(1, ""));
		this->SetText(newText);
	}

	void Clipboard::_ClearData(const ValueList& args, SharedValue result)
	{
		// TODO: Support the mime-type parameter.
		this->ClearData();
	}

	void Clipboard::_SetText(const ValueList& args, SharedValue result)
	{
		args.VerifyException("setText", "s");
		std::string newText(args.GetString(0, ""));
		this->SetText(newText);
	}

	void Clipboard::_GetText(const ValueList& args, SharedValue result)
	{
		result->SetString(this->GetText());
	}

	void Clipboard::_ClearText(const ValueList& args, SharedValue result)
	{
		this->ClearText();
	}

	void Clipboard::_HasText(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->HasText());
	}

	void Clipboard::SetText(std::string& newText)
	{
		this->SetTextImpl(newText);
	}

	std::string& Clipboard::GetText()
	{
		return this->GetTextImpl();
	}

	void Clipboard::ClearText()
	{
		this->ClearData();
	}

	bool Clipboard::HasText()
	{
		return this->HasTextImpl();
	}

	void Clipboard::ClearData()
	{
		// TODO: Eventually this should allow clearing based on mime-type.
		this->ClearDataImpl();
	}
}

