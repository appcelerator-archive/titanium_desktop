/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../ui_module.h"

namespace ti
{
	static NSString* GetBestPasteboardTextType()
	{
		NSArray *types = [NSArray arrayWithObjects:
			NSStringPboardType, NSRTFDPboardType, NSRTFPboardType, nil];
		return [[NSPasteboard generalPasteboard] availableTypeFromArray:types];
	}

	Clipboard::~Clipboard()
	{

	}

	void Clipboard::SetTextImpl(std::string& newText)
	{
		NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
		NSArray *types = [NSArray arrayWithObjects: NSStringPboardType, NSRTFPboardType, nil];
		[pasteboard declareTypes:types owner:nil];

		NSString* nsstring = [NSString stringWithUTF8String:newText.c_str()];
		[pasteboard setString:nsstring forType:NSStringPboardType];

		// We could possibly use setString again for the RTF type. I'm
		// just doing this to do be safe. -- Martin
		NSAttributedString* attributedString = [[NSAttributedString alloc]
			initWithString:nsstring];
		NSData* attributedStringData = [attributedString 
			RTFFromRange:NSMakeRange(0, [attributedString length]) 
			documentAttributes:nil];
		[pasteboard setData:attributedStringData forType:NSRTFPboardType];
	}

	std::string& Clipboard::GetTextImpl()
	{
		static std::string clipboardText;
		NSString* type = GetBestPasteboardTextType();
		NSData* data = nil;

		@try
		{
			data = [[NSPasteboard generalPasteboard] dataForType:type];
		}
		@catch (NSException *e)
		{
			throw ValueException::FromFormat("Could not get clipboard contents: %s",
				[[e reason] UTF8String]);
		}

		if (data == nil)
		{
			clipboardText = "";
		}
		else if (type == NSStringPboardType)
		{
			NSString* string = [[NSString alloc]
				initWithData:data encoding:NSUTF8StringEncoding];
			clipboardText = [string UTF8String];
		}
		else if (type == NSRTFDPboardType)
		{
			NSAttributedString* attributedString = [[NSAttributedString alloc]
				initWithRTFD:data documentAttributes:NULL];
			clipboardText = [[attributedString string] UTF8String];
		}
		else if (type == NSRTFPboardType)
		{
			NSAttributedString* attributedString = [[NSAttributedString alloc]
				initWithRTF:data documentAttributes:NULL];
			clipboardText = [[attributedString string] UTF8String];
		}

		return clipboardText;
	}

	bool Clipboard::HasTextImpl()
	{
		return GetBestPasteboardTextType() != nil;
	}

	void Clipboard::ClearTextImpl()
	{
		[[NSPasteboard generalPasteboard] declareTypes:[NSArray array] owner:nil];
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
