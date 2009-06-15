#ifndef TI_UPDATE_DIALOG_H
#define TI_UPDATE_DIALOG_H

#include "Dialog.h"
#include <commctrl.h>
#include <string>
#include <vector>
#include "InstallJob.h"

namespace ti
{
	class UpdateDialog : public Dialog
	{
		protected:
			HWND installButton, cancelButton, updateList;
			std::vector<InstallJob*> jobs;
			
			virtual void Initialize(HWND window);
			virtual void ButtonClicked(int id);
			HTREEITEM InsertUpdate(std::string name, std::string newVersion);
		public:
			UpdateDialog(std::vector<InstallJob*> jobs);
			
	};

}
#endif