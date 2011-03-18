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

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import <WebKit/WebScriptObject.h>

#include <kroll/kroll.h>

#include "NativeWindow.h"

@class NativeWindow;
@interface WebViewDelegate : NSObject
{
    Titanium::NativeWindow* window;
    BOOL initialDisplay;
    std::map<WebFrame*, KObjectRef>* frameToGlobalObject;
    Logger* logger;
}

-(id)initWithWindow:(Titanium::NativeWindow*)window;
-(void)setupPreferences;
-(void)registerGlobalObject:(KObjectRef) globalObject forFrame:(WebFrame *)frame;
-(KObjectRef)registerJSContext:(JSGlobalContextRef)context forFrame:(WebFrame*)frame;
-(BOOL)isGlobalObjectRegisteredForFrame:(WebFrame*) frame;
-(KObjectRef)globalObjectForFrame:(WebFrame*) frame;
-(void)deregisterGlobalObjectForFrame:(WebFrame *)frame;

@end
