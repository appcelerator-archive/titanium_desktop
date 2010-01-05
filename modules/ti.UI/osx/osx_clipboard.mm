/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "../ui_module.h"

namespace ti
{
	NSArray* textTypes = [NSArray arrayWithObjects:
			NSStringPboardType, NSRTFDPboardType, NSRTFPboardType, nil];
	NSArray *uriListTypes = [NSArray arrayWithObjects:NSFilenamesPboardType,
			NSURLPboardType, nil];

	static NSString* GetBestPasteboardTextType()
	{
		return [[NSPasteboard generalPasteboard] availableTypeFromArray:textTypes];
	}

	static NSString* GetBestPasteboardURIListType()
	{
		return [[NSPasteboard generalPasteboard] availableTypeFromArray:uriListTypes];
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

		if (!type)
		{
			clipboardText = "";
			return clipboardText;
		}

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
		NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
		NSArray* types = [pasteboard types];

		// Need to remove all types from the pasteboard, except for
		// non-text types that we could be potentially leaving there.
		NSMutableArray* newTypes = [[NSMutableArray alloc] init];
		for (unsigned int i = 0; i < [types count]; i++)
		{
			NSString* type = [types objectAtIndex:i];
			if ([uriListTypes containsObject:type])
				[newTypes addObject:type];
		}

		[pasteboard declareTypes:newTypes owner:nil];
	}

	BlobRef Clipboard::GetImageImpl(std::string& mimeType)
	{
		BlobRef image(0);
		return image;
	}

	void Clipboard::SetImageImpl(std::string& mimeType, BlobRef image)
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

		NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
		NSString* type = GetBestPasteboardURIListType();
		if (type == NSURLPboardType)
		{
			NSURL* url = [NSURL URLFromPasteboard:pasteboard];
			if (url)
				uriList.push_back([[url absoluteString] UTF8String]);
		}
		else
		{
			NSArray* filenamesArray = [pasteboard 
				propertyListForType:NSFilenamesPboardType];
			if (!filenamesArray)
				return uriList;

			for (unsigned int i = 0; i < [filenamesArray count]; i++)
			{
				uriList.push_back(URLUtils::PathToFileURL(
					[[filenamesArray objectAtIndex:i] UTF8String]));
			}
		}

		return uriList;
	}

	void Clipboard::SetURIListImpl(std::vector<std::string>& uriList)
	{
		bool wroteToURLSlot = false;
		NSMutableArray* filenamesArray = [[NSMutableArray alloc] init];
		NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];

		for (unsigned int i = 0; i < uriList.size(); i++)
		{
			NSURL* url = [[NSURL alloc] initWithString:
				[NSString stringWithUTF8String:uriList[i].c_str()]];

			if (!url)
				continue;

			// Only write the first available URL to the pasteboard.
			if (!wroteToURLSlot)
			{
				[pasteboard addTypes:[NSArray arrayWithObject:NSURLPboardType] owner:nil];
				[url writeToPasteboard:pasteboard];
				wroteToURLSlot = true;
			}

			if ([url isFileURL])
			{
				[filenamesArray addObject:[url path]];
			}
		}

		if ([filenamesArray count] > 0)
		{
			[pasteboard addTypes:[NSArray arrayWithObject:NSFilenamesPboardType] owner:nil];
			[pasteboard setPropertyList:filenamesArray forType:NSFilenamesPboardType];
		}
	}

	bool Clipboard::HasURIListImpl()
	{
		return GetBestPasteboardURIListType() != nil;
	}

	void Clipboard::ClearURIListImpl()
	{
		NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
		NSArray* types = [pasteboard types];

		// Need to remove all types from the pasteboard, except for
		// non-URI-list types that we could be potentially leaving there.
		NSMutableArray* newTypes = [[NSMutableArray alloc] init];
		for (unsigned int i = 0; i < [types count]; i++)
		{
			NSString* type = [types objectAtIndex:i];
			if ([textTypes containsObject:type])
			{
				[newTypes addObject:type];
			}
		}

		[pasteboard declareTypes:newTypes owner:nil];
	}
}
