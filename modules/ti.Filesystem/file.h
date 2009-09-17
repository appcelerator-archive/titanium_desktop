/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _TI_FILE_H_
#define _TI_FILE_H_
#include <kroll/kroll.h>

#ifdef OS_WIN32
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#elif OS_OSX
#import <Foundation/Foundation.h>
#endif

#include "file_stream.h" // TODO remove this include when read/write methods are removed from File class
#include <string>

namespace ti
{
	class File : public StaticBoundObject
	{
		public:
		File(std::string filename);
		virtual ~File();

		std::string& GetFilename() { return this->filename; }
		virtual SharedString DisplayString(int levels=3)
		{
			return new string(GetFilename());
		}

		private:
		std::string filename;
		ti::FileStream* readLineFS;

		void ToString(const ValueList& args, SharedValue result);
		void ToURL(const ValueList& args, SharedValue result);
		void IsFile(const ValueList& args, SharedValue result);
		void IsDirectory(const ValueList& args, SharedValue result);
		void IsHidden(const ValueList& args, SharedValue result);
		void IsSymbolicLink(const ValueList& args, SharedValue result);
		void IsExecutable(const ValueList& args, SharedValue result);
		void IsReadonly(const ValueList& args, SharedValue result);
		void IsWriteable(const ValueList& args, SharedValue result);
		void Resolve(const ValueList& args, SharedValue result);
		void Write(const ValueList& args, SharedValue result);
		void Read(const ValueList& args, SharedValue result);
		void ReadLine(const ValueList& args, SharedValue result);
		void Copy(const ValueList& args, SharedValue result);
		void Move(const ValueList& args, SharedValue result);
		void Rename(const ValueList& args, SharedValue result);
		void CreateDirectory(const ValueList& args, SharedValue result);
		void DeleteDirectory(const ValueList& args, SharedValue result);
		void DeleteFile(const ValueList& args, SharedValue result);
		void Equals(const ValueList& args, SharedValue result);
		void GetDirectoryListing(const ValueList& args, SharedValue result);
		void GetParent(const ValueList& args, SharedValue result);
		void GetExists(const ValueList& args, SharedValue result);
		void GetCreateTimestamp(const ValueList& args, SharedValue result);
		void GetModificationTimestamp(const ValueList& args, SharedValue result);
		void GetName(const ValueList& args, SharedValue result);
		void GetExtension(const ValueList& args, SharedValue result);
		void GetNativePath(const ValueList& args, SharedValue result);
		void GetSize(const ValueList& args, SharedValue result);
		void GetSpaceAvailable(const ValueList& args, SharedValue result);
		void CreateShortcut(const ValueList& args, SharedValue result);
		void SetExecutable(const ValueList& args, SharedValue result);
		void SetReadonly(const ValueList& args, SharedValue result);
		void SetWriteable(const ValueList& args, SharedValue result);
		void Unzip(const ValueList& args, SharedValue result);
	};
}

#endif 
