/*
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../Application.h"

#include <Foundation/Foundation.h>

@interface KrollExitNotificationListener : NSObject
@end

@implementation KrollExitNotificationListener

-(void)krollExitNotification:(id)event
{
    Host* host = Host::GetInstance();
    host->Exit(0);
}

@end

namespace Titanium {

void Application::Restart(const ValueList& args, KValueRef result)
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

void Application::Setup() 
{
    // We register a generic notification listener for listening for KrollExit events
    // which come from the boot when we have no active windows in the responder chain
    // and the NSApplication delegate receives the terminate.  
    KrollExitNotificationListener *listener = [[KrollExitNotificationListener alloc] init];
    [[NSNotificationCenter defaultCenter] addObserver:listener selector:@selector(krollExitNotification:) name:@"KrollExit" object:nil];
    [listener release];
}

} // namespace Titanium
