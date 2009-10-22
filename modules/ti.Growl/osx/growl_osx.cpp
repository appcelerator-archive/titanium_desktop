/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "growl_osx.h"
#import "GrowlApplicationBridge.h"
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace kroll;
using namespace ti;

namespace ti
{

	GrowlOSX::GrowlOSX(KObjectRef global) :
		GrowlBinding(global)
	{
		//Delegate will have a retain count of one from the alloc init.
		delegate = [[TiGrowlDelegate alloc] init];
	}

	GrowlOSX::~GrowlOSX()
	{
		[delegate release];
	}

	void GrowlOSX::ShowNotification(std::string& title, std::string& description,
		std::string& iconURL, int notification_delay, KMethodRef callback)
	{
		std::string myIconURL = iconURL;
		NSData *iconData = nil;

		if (!iconURL.empty())
		{
			std::string iconPath = URLUtils::URLToPath(iconURL);

			// If the path was not modified, it's still a URL.
			if (iconPath != iconURL)
			{
				myIconURL = URLUtils::PathToFileURL(iconPath);
			}

			NSURL* iconNSURL = [NSURL URLWithString:
				[NSString stringWithUTF8String:myIconURL.c_str()]];
			iconData = [NSData dataWithContentsOfURL:iconNSURL];
		}

		//NSMutableArray* clickContext = [[NSMutableArray alloc] init];
		//[clickContext addObject:[[MethodWrapper alloc] initWithMethod:new KMethodRef(callback)]];

		const char* titleCString = title.c_str();
		NSString * titleString = nil;
		if (titleCString != NULL) {
			titleString = [NSString stringWithUTF8String:titleCString];
		}
		
		const char * descriptionCString = description.c_str();
		NSString * descriptionString = nil;
		if (descriptionCString != NULL) {
			descriptionString = [NSString stringWithUTF8String:descriptionCString];
		}

		[GrowlApplicationBridge
			notifyWithTitle:titleString
			description:descriptionString
			notificationName:@"tiNotification"
			iconData:iconData
			priority:0
			isSticky:NO
			clickContext:nil];
	}

	bool GrowlOSX::IsRunning()
	{
		return [GrowlApplicationBridge isGrowlRunning];
	}

	void GrowlOSX::CopyToApp(kroll::Host *host, kroll::Module *module)
	{
		std::string dir = host->GetApplicationHomePath() + KR_PATH_SEP + "Contents" +
			KR_PATH_SEP + "Frameworks" + KR_PATH_SEP + "Growl.framework";

		if (!FileUtils::IsDirectory(dir))
		{
			NSFileManager *fm = [NSFileManager defaultManager];
			NSString *src = [NSString stringWithFormat:@"%s/Resources/Growl.framework", module->GetPath().c_str()];
			NSString *dest = [NSString stringWithFormat:@"%s/Contents/Frameworks", host->GetApplicationHomePath().c_str()];
			[fm copyPath:src toPath:dest handler:nil];

			src = [NSString stringWithFormat:@"%s/Resources/Growl Registration Ticket.growlRegDict", module->GetPath().c_str()];
			dest = [NSString stringWithFormat:@"%s/Contents/Resources", host->GetApplicationHomePath().c_str()];
			[fm copyPath:src toPath:dest handler:nil];
		}
	}
}
