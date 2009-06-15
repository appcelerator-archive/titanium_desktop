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

namespace ti {
	GrowlOSX::GrowlOSX(SharedKObject global) : GrowlBinding(global) {
		delegate = [[TiGrowlDelegate alloc] init];
		//Delegate will have a retain count of one from the alloc init.
	}

	GrowlOSX::~GrowlOSX() {
		[delegate release];
	}


	void * GrowlOSX::URLWithAppCString(const char * inputCString)
	{
		if (inputCString == NULL) return nil;
		NSString * inputString = [NSString stringWithUTF8String:inputCString];
		NSURL * result = [NSURL URLWithString:inputString];
		NSString * scheme = [result scheme];
		
		BOOL isAppPath = [scheme isEqualToString:@"app"] || [scheme isEqualToString:@"ti"];
		if ((scheme != nil) && !isAppPath) return result;
		
		NSString * resourceSpecifier = [result resourceSpecifier];
		if (isAppPath || [resourceSpecifier hasPrefix:@"//"]){
			resourceSpecifier = [resourceSpecifier substringFromIndex:2];
		}
		
		SharedValue iconPathValue = global->CallNS("App.appURLToPath", Value::NewString([resourceSpecifier UTF8String]));
		if (!(iconPathValue->IsString())) return nil;
		
		std::string iconPath = iconPathValue->ToString();
		const char * iconPathCString = iconPath.c_str();
		
		if (iconPathCString == NULL) return nil;
		return [NSURL fileURLWithPath:[NSString stringWithUTF8String:iconPathCString]];
	}
	

	void GrowlOSX::ShowNotification(std::string& title, std::string& description, std::string& iconURL, int notification_delay, SharedKMethod callback)
	{
		NSData *iconData = nil;

		if (iconURL.size() > 0)
		{
			const char * iconURLCString = iconURL.c_str();
			//TODO: This should be more generalized and using centralized methods, but for now,

			NSURL * iconNSURL = (NSURL *)URLWithAppCString(iconURLCString);

			if ([iconNSURL isFileURL]){
				iconData = [NSData dataWithContentsOfURL:iconNSURL];
			} else if (iconNSURL != nil){
				iconData = [NSData dataWithContentsOfURL:iconNSURL];
				//TODO: Delayed load and fire.
			}
		}

		//NSMutableArray* clickContext = [[NSMutableArray alloc] init];
		//[clickContext addObject:[[MethodWrapper alloc] initWithMethod:new SharedKMethod(callback)]];

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
