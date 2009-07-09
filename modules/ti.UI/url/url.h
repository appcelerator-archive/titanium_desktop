/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef TITANIUM_URL_H_
#define TITANIUM_URL_H_
namespace ti
{
	std::string TiURLToPath(string tiURL);
	std::string AppURLToPath(string appURL);
	std::string NormalizeAppURL(string url);
	void NormalizeURLCallback(const char* url, char* buffer, int bufferLength);
	void URLToFileURLCallback(const char* url, char* buffer, int bufferLength);
	std::string URLToPathOrURL(string& url);
}
#endif
