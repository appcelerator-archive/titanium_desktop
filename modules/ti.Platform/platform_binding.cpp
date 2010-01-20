/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include <Poco/Environment.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Net/NetworkInterface.h>
#include <Poco/Net/IPAddress.h>
#include "platform_binding.h"

namespace ti
{

PlatformBinding::PlatformBinding() :
	KAccessorObject("Platform")
{
	/**
	 * @tiapi(method=True,name=Platform.getOSType,since=0.9)
	 * @tiresult[String] The architecture type of the operating system (either '32bit' or '64bit')
	 */
	this->SetMethod("getType", &PlatformBinding::_GetType);
	this->SetMethod("getOSType", &PlatformBinding::_GetType); // COMPATIBILTY.

	/**
	 * @tiapi(method=True,name=Platform.getName,since=0.9)
	 * @tiresult[String] The operating system name.
	 */
	this->SetMethod("getName", &PlatformBinding::_GetName); 

	/**
	 * @tiapi(method=True,name=Platform.getVersion,since=0.9)
	 * @tiresult[String] The operating system version.
	 */
	this->SetMethod("getVersion", &PlatformBinding::_GetVersion);

	/**
	 * @tiapi(method=True,name=Platform.getArchitecture,since=0.9)
	 * @tiresult[String] The processor architecture.
	 */
	this->SetMethod("getArchitecture", &PlatformBinding::_GetArchitecture);

	/**
	 * @tiapi(method=True,name=Platform.getProcessorCount,since=0.9)
	 * @tiresult[Number] The number of processors this machine has.
	 */
	this->SetMethod("getProcessorCount", &PlatformBinding::_GetProcessorCount);

	/**
	 * @tiapi(method=True,name=Platform.getUsername,since=0.9)
	 * @tiresult[String] The username of the current user.
	 */
	this->SetMethod("getUsername", &PlatformBinding::_GetUsername);

	/**
	 * @tiapi(method=True,name=Platform.getMachineId,since=0.9)
	 * @tiresult[String] An id which uniquely identifies this user and operating system installation.
	 */
	this->SetMethod("getMachineId", &PlatformBinding::_GetMachineId);
	this->SetMethod("getId", &PlatformBinding::_GetMachineId); // COMPATIBILITY.

	/**
	 * @tiapi(method=True,returns=String,name=Platform.createUUID,since=0.3)
	 * Create a universally unique identifier string.
	 * @tiresult[String] a new UUID.
	 */
	this->SetMethod("createUUID", &PlatformBinding::_CreateUUID);

	/**
	 * @tiapi(method=True,name=Platform.openApplication,since=0.9)
	 * Open an application or file using the default system action.
	 * @tiarg[String,name] The name or path to the application or file to open.
	 */
	this->SetMethod("openApplication", &PlatformBinding::_OpenApplication);

	/**
	 * @tiapi(method=True,name=Platform.openURL,since=0.2)
	 * Open a URL in the default system browser.
	 * @tiarg[String,url] the URL to open.
	 */
	this->SetMethod("openURL", &PlatformBinding::_OpenURL);

	/**
	 * @tiapi(method=True,name=Platform.takeScreenshot,platforms=osx|win32,since=0.4)
	 * Take a screenshot of the desktop and save it to a file.
	 * @tiarg[String,filename] The filename to write the image to.
	 */
	this->SetMethod("takeScreenshot", &PlatformBinding::_TakeScreenshot);
}

PlatformBinding::~PlatformBinding()
{
}

void PlatformBinding::_GetName(const ValueList& args, KValueRef result)
{
	result->SetString(Poco::Environment::osName().c_str());
}

void PlatformBinding::_GetVersion(const ValueList& args, KValueRef result)
{
	static std::string osVersion;
	if (osVersion.empty())
		osVersion = GetVersionImpl();

	result->SetString(osVersion.c_str());
}

void PlatformBinding::_GetType(const ValueList& args, KValueRef result)
{
	result->SetString(OS_TYPE);
}

void PlatformBinding::_GetProcessorCount(const ValueList& args, KValueRef result)
{
	result->SetInt(GetProcessorCountImpl());
}

void PlatformBinding::_GetArchitecture(const ValueList& args, KValueRef result)
{
	result->SetString(Poco::Environment::osArchitecture().c_str());
}

void PlatformBinding::_GetMachineId(const ValueList& args, KValueRef result)
{
	result->SetString(PlatformUtils::GetMachineId().c_str());
}

void PlatformBinding::_GetUsername(const ValueList& args, KValueRef result)
{
	result->SetString(PlatformUtils::GetUsername().c_str());
}

void PlatformBinding::_CreateUUID(const ValueList& args, KValueRef result)
{
	result->SetString(DataUtils::GenerateUUID().c_str());
}

void PlatformBinding::_OpenApplication(const ValueList& args, KValueRef result)
{
	args.VerifyException("openApplication", "s");
	result->SetBool(this->OpenApplicationImpl(args.at(0)->ToString()));
}

void PlatformBinding::_OpenURL(const ValueList& args, KValueRef result)
{
	args.VerifyException("openURL", "s");
	result->SetBool(this->OpenURLImpl(args.at(0)->ToString()));
}

void PlatformBinding::_TakeScreenshot(const ValueList& args, KValueRef result)
{
	args.VerifyException("takeScreenshot", "s");
	this->TakeScreenshotImpl(args.at(0)->ToString());
}

}
