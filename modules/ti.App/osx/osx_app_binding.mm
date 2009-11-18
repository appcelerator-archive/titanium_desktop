/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include "../app_binding.h"
#include <Foundation/Foundation.h>

namespace ti
{

void AppBinding::Restart(const ValueList& args, KValueRef result)
{
	Host* host = Host::GetInstance();
	std::string cmdline(host->GetApplication()->arguments.at(0));

	NSProcessInfo* p = [NSProcessInfo processInfo];
	NSString* path = [[NSBundle mainBundle] bundlePath];
	NSString* killArg1AndOpenArg2Script = [NSString 
		stringWithFormat:@"kill -9 %d\n open \"%@\"",
		[p processIdentifier], path];

	// -c tells sh to execute the next argument, passing it the remaining arguments.
	NSArray* shArgs = [NSArray arrayWithObjects:@"-c",
		killArg1AndOpenArg2Script, nil];
	NSTask* restartTask = [NSTask launchedTaskWithLaunchPath:@"/bin/sh" arguments:shArgs];
	[restartTask waitUntilExit]; //wait for killArg1AndOpenArg2Script to finish

	host->Exit(0);
}

}
