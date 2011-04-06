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

#include "Platform.h"

#include <kroll/kroll.h>
#include <Poco/Environment.h>

namespace Titanium {

Platform::Platform() :
    KAccessorObject("Platform")
{
    /**
     * @tiapi(method=True,name=Platform.getOSType,since=0.9)
     * @tiresult[String] The architecture type of the operating system (either '32bit' or '64bit')
     */
    this->SetMethod("getType", &Platform::_GetType);
    this->SetMethod("getOSType", &Platform::_GetType); // COMPATIBILTY.

    /**
     * @tiapi(method=True,name=Platform.getName,since=0.9)
     * @tiresult[String] The operating system name.
     */
    this->SetMethod("getName", &Platform::_GetName); 

    /**
     * @tiapi(method=True,name=Platform.getVersion,since=0.9)
     * @tiresult[String] The operating system version.
     */
    this->SetMethod("getVersion", &Platform::_GetVersion);

    /**
     * @tiapi(method=True,name=Platform.getArchitecture,since=0.9)
     * @tiresult[String] The processor architecture.
     */
    this->SetMethod("getArchitecture", &Platform::_GetArchitecture);

    /**
     * @tiapi(method=True,name=Platform.getProcessorCount,since=0.9)
     * @tiresult[Number] The number of processors this machine has.
     */
    this->SetMethod("getProcessorCount", &Platform::_GetProcessorCount);

    /**
     * @tiapi(method=True,name=Platform.getUsername,since=0.9)
     * @tiresult[String] The username of the current user.
     */
    this->SetMethod("getUsername", &Platform::_GetUsername);

    /**
     * @tiapi(method=True,name=Platform.getMachineId,since=0.9)
     * @tiresult[String] An id which uniquely identifies this user and operating system installation.
     */
    this->SetMethod("getMachineId", &Platform::_GetMachineId);
    this->SetMethod("getId", &Platform::_GetMachineId); // COMPATIBILITY.

    /**
     * @tiapi(method=True,returns=String,name=Platform.createUUID,since=0.3)
     * Create a universally unique identifier string.
     * @tiresult[String] a new UUID.
     */
    this->SetMethod("createUUID", &Platform::_CreateUUID);

    /**
     * @tiapi(method=True,name=Platform.openApplication,since=0.9)
     * Open an application or file using the default system action.
     * @tiarg[String,name] The name or path to the application or file to open.
     */
    this->SetMethod("openApplication", &Platform::_OpenApplication);

    /**
     * @tiapi(method=True,name=Platform.openURL,since=0.2)
     * Open a URL in the default system browser.
     * @tiarg[String,url] the URL to open.
     */
    this->SetMethod("openURL", &Platform::_OpenURL);

    /**
     * @tiapi(method=True,name=Platform.takeScreenshot,platforms=osx|win32,since=0.4)
     * Take a screenshot of the desktop and save it to a file.
     * @tiarg[String,filename] The filename to write the image to.
     */
    this->SetMethod("takeScreenshot", &Platform::_TakeScreenshot);
}

Platform::~Platform()
{
}

void Platform::_GetName(const ValueList& args, KValueRef result)
{
    result->SetString(Poco::Environment::osName().c_str());
}

void Platform::_GetVersion(const ValueList& args, KValueRef result)
{
    static std::string osVersion;
    if (osVersion.empty())
        osVersion = GetVersionImpl();

    result->SetString(osVersion.c_str());
}

void Platform::_GetType(const ValueList& args, KValueRef result)
{
    result->SetString(OS_TYPE);
}

void Platform::_GetProcessorCount(const ValueList& args, KValueRef result)
{
    result->SetInt(PlatformUtils::GetProcessorCount());
}

void Platform::_GetArchitecture(const ValueList& args, KValueRef result)
{
    result->SetString(Poco::Environment::osArchitecture().c_str());
}

void Platform::_GetMachineId(const ValueList& args, KValueRef result)
{
    result->SetString(PlatformUtils::GetMachineId().c_str());
}

void Platform::_GetUsername(const ValueList& args, KValueRef result)
{
    result->SetString(PlatformUtils::GetUsername().c_str());
}

void Platform::_CreateUUID(const ValueList& args, KValueRef result)
{
    result->SetString(DataUtils::GenerateUUID().c_str());
}

void Platform::_OpenApplication(const ValueList& args, KValueRef result)
{
    args.VerifyException("openApplication", "s");
    result->SetBool(this->OpenApplicationImpl(args.at(0)->ToString()));
}

void Platform::_OpenURL(const ValueList& args, KValueRef result)
{
    args.VerifyException("openURL", "s");
    result->SetBool(this->OpenURLImpl(args.at(0)->ToString()));
}

void Platform::_TakeScreenshot(const ValueList& args, KValueRef result)
{
    args.VerifyException("takeScreenshot", "s");
    this->TakeScreenshotImpl(args.at(0)->ToString());
}

} // namespace Titanium
