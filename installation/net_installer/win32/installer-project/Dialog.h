#ifndef TI_DIALOG_H
#define TI_DIALOG_H

#include <windows.h>

namespace ti
{
	class Dialog {
		protected:
			HWND window;
			static BOOL CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
			void Create(int id);
			
			virtual void Initialize(HWND window);
			virtual void ButtonClicked(int id) = 0;
			
		public:
			Dialog();
			virtual ~Dialog() {}
			
			void Destroy();
			HWND GetWindow() { return window; }
	};
}

#endif