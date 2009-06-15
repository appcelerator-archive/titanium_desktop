/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef SNARL_INTERFACE
#define SNARL_INTERFACE

#include <windows.h>
#include <mbstring.h>
#include <string>

class SnarlInterface {
	public:
        static const int SNARL_STRING_LENGTH = 1024;
		static const int SNARL_UNICODE_LENGTH = SNARL_STRING_LENGTH / 2;

		static const int SNARL_LAUNCHED = 1;
		static const int SNARL_QUIT = 2;

		static const int SNARL_NOTIFICATION_CLICKED = 32;
		static const int SNARL_NOTIFICATION_TIMED_OUT = 33;
		static const int SNARL_NOTIFICATION_ACK = 34;

		enum SNARL_COMMANDS {
			SNARL_SHOW = 1,
			SNARL_HIDE,
			SNARL_UPDATE,
			SNARL_IS_VISIBLE,
			SNARL_GET_VERSION,
			SNARL_REGISTER_CONFIG_WINDOW,
			SNARL_REVOKE_CONFIG_WINDOW,
			SNARL_REGISTER_ALERT,
			SNARL_REVOKE_ALERT,
			SNARL_REGISTER_CONFIG_WINDOW_2,
			SNARL_GET_VERSION_EX,

			SNARL_EX_SHOW = 0x20
		};

		typedef struct SNARLSTRUCT{
			SNARL_COMMANDS cmd;
			long id;
			long timeout;
			long lngData2;
			char title[SNARL_STRING_LENGTH];
			char text[SNARL_STRING_LENGTH];
			char icon[SNARL_STRING_LENGTH];
		};

		struct SNARLSTRUCTEX {
			SNARL_COMMANDS cmd;
			long id;
			long timeout;
			long lngData2;
			char title[SNARL_STRING_LENGTH];
			char text[SNARL_STRING_LENGTH];
			char icon[SNARL_STRING_LENGTH];
			char snarlClass[SNARL_STRING_LENGTH];
			char extra[SNARL_STRING_LENGTH];
			char extra2[SNARL_STRING_LENGTH];
			long reserved1;
			long reserved2;
		};

		SnarlInterface();
		~SnarlInterface();
		long snShowMessage(std::wstring title, std::wstring text, long timeout, std::wstring iconPath, HWND hWndReply, long uReplyMsg);
		long snShowMessageEx(std::wstring title, std::wstring text, long timeout, std::wstring iconPath, HWND hWndReply, long uReplyMsg, std::wstring soundPath, std::wstring msgClass);
		bool snHideMessage(long id);
		bool snIsMessageVisible(long id);
		bool snUpdateMessage(long id, std::wstring title, std::wstring text, std::wstring icon);
		bool snRegisterConfig(HWND hWnd, std::wstring appName, long replyMsg);
		bool snRegisterConfig2(HWND hWnd, std::wstring appName, long replyMsg, std::wstring icon);
		bool snRevokeConfig(HWND hWnd);
		bool snRegisterAlert(std::wstring appName, std::wstring alertName);
		bool snGetVersion(int* major, int* minor);
		long snGetVersionEx();
		static HWND snGetSnarlWindow();
		long snGetGlobalMsg();

		std::string snGetIconsPath();
		std::string snGetAppPath();

		static std::wstring convertTowstring(LPSTR str);
		static LPSTR convertToMultiByte(std::wstring str, int *len);

		template <class T>
		static long send(T snarlStruct);
	private:
		std::string SNARL_GLOBAL_MESSAGE;

		std::string getPath(std::string fileName);
};

#endif // SNARL_INTERFACE
