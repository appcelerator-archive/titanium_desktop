/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

#include <kroll/kroll.h>

namespace ti
{
	class Clipboard : public AccessorBoundObject
	{
		public:
		Clipboard();
		~Clipboard();

		void _GetData(const ValueList& args, SharedValue result);
		void _SetData(const ValueList& args, SharedValue result);
		void _ClearData(const ValueList& args, SharedValue result);
		void _GetText(const ValueList& args, SharedValue result);
		void _SetText(const ValueList& args, SharedValue result);
		void _ClearText(const ValueList& args, SharedValue result);
		void _HasText(const ValueList& args, SharedValue result);

		std::string& GetText();
		void SetText(std::string& newText);
		bool HasText();
		void ClearText();
		void ClearData();

		std::string& GetTextImpl();
		void SetTextImpl(std::string& newText);
		bool HasTextImpl();
		void ClearDataImpl();
	};
}
#endif
