#ifndef TI_INTRO_DIALOG_H
#define TI_INTRO_DIALOG_H

#include "dialog.h"

namespace ti
{
	class IntroDialog : public Dialog
	{
		protected:
			HWND nameLabel;
			HWND versionLabel;
			HWND publisherLabel;
			HWND urlLabel;
			HWND licenseBlurb;
			HWND licenseTextBox;
			HWND installBox;
			HWND installLocationText;
			HWND installLocationButton;
			HWND startMenuCheck;
			HWND securityBlurb;
			HWND runButton;
			HWND cancelButton;

			void InstallLocationClicked();
			virtual void Initialize(HWND window);
			virtual void ButtonClicked(int id);
			
		public:
			IntroDialog();
			
	};
}
#endif
