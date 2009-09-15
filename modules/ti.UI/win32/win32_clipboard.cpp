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
	}

	std::string& Clipboard::GetTextImpl()
	{
		static std::string clipboardText;
		return clipboardText;
	}

	bool Clipboard::HasTextImpl()
	{
		return false;
	}

	void Clipboard::ClearDataImpl()
	{
	}
}
