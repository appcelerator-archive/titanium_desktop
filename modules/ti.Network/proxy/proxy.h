/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 * 
 * @Author: Mital Vora
 */
#ifndef _TI_PROXY_H_
#define _TI_PROXY_H_
#include <kroll/kroll.h>

#ifdef OS_WIN32
#include <windows.h>
#include <commdlg.h>
//#include <shellapi.h>
//#include <shlobj.h>
#elif OS_OSX
#import <Foundation/Foundation.h>
#endif


#include <string>

namespace ti
{
	class Proxy : public StaticBoundObject
	{
		public:
			Proxy(const std::string& _hostname,
				const std::string& _port,
				const std::string& _username = "",
				const std::string& _password = "");
			virtual ~Proxy();

		private:

			const std::string hostname;
			const std::string port;
			const std::string username;
			const std::string password;

			/**
			 * Function: getHostName
			 *   return the HostName
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   return the HostName
			 */
			void getHostName(const ValueList& args, SharedValue result);

			/**
			 * Function: getPort
			 *   return the Port
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   return the Port
			 */
			void getPort(const ValueList& args, SharedValue result);

			/**
			 * Function: getUserName
			 *   return the UserName
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   return the UserName
			 */
			void getUserName(const ValueList& args, SharedValue result);

			/**
			 * Function: getPassword
			 *   return the Password
			 *
			 * Parameters:
			 *
			 * Returns:
			 *   return the Password
			 */
			void getPassword(const ValueList& args, SharedValue result);

	};
}

#endif /* _TI_PROXY_H_ */
