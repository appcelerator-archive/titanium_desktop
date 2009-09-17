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

		enum DataType { TEXT, URI_LIST, IMAGE, UNKNOWN };
		void _GetData(const ValueList& args, SharedValue result);
		void _SetData(const ValueList& args, SharedValue result);
		void _ClearData(const ValueList& args, SharedValue result);
		void _HasData(const ValueList& args, SharedValue result);
		void _GetText(const ValueList& args, SharedValue result);
		void _SetText(const ValueList& args, SharedValue result);
		void _ClearText(const ValueList& args, SharedValue result);
		void _HasText(const ValueList& args, SharedValue result);
		bool HasData(DataType type);
		void ClearData(DataType type);

		std::string& GetText();
		void SetText(std::string& newText);
		bool HasText();
		void ClearText();
		AutoBlob GetImage(std::string& mimeType);
		void SetImage(std::string& mimeType, AutoBlob newImage);
		bool HasImage();
		void ClearImage();
		std::vector<std::string>& GetURIList();
		void SetURIList(std::vector<std::string>& newURIList);
		bool HasURIList();
		void ClearURIList();

		std::string& GetTextImpl();
		void SetTextImpl(std::string& newText);
		bool HasTextImpl();
		void ClearTextImpl();
		AutoBlob GetImageImpl(std::string& mimeType);
		void SetImageImpl(std::string& mimeType, AutoBlob image);
		bool HasImageImpl();
		void ClearImageImpl();
		std::vector<std::string>& GetURIListImpl();
		void SetURIListImpl(std::vector<std::string>& uriList);
		bool HasURIListImpl();
		void ClearURIListImpl();
	};
}
#endif
