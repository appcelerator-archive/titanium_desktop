/*
 * Copyright (c) 2008-2010 Appcelerator, Inc. All Rights Reserved.
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

#import <AppKit/NSApplication.h>

#include "UIMac.h"

@interface TitaniumApplicationDelegate : NSObject<NSApplicationDelegate>
{
    Titanium::UIMac *binding;
}

-(id)initWithBinding:(Titanium::UIMac*)binding;

-(BOOL)application:(NSApplication*)theApplication openFile:(NSString*)filename;
-(BOOL)application:(NSApplication*)theApplication openFiles:(NSArray*)filenames;
-(NSApplicationTerminateReply) applicationShouldTerminate:(NSApplication*)sender;

@end
