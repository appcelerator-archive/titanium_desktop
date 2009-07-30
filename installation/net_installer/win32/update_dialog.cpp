
#include "update_dialog.h"
#include "Resource.h"
#include <commctrl.h>
#include <iostream>
#include "api/utils/utils.h"

extern bool doInstall;
extern KrollUtils::SharedApplication app;

namespace ti
{
	UpdateDialog::UpdateDialog(std::vector<InstallJob*> jobs) : Dialog(), jobs(jobs)
	{
		Create(IDD_UPDATEDIALOG);
	}

	HTREEITEM UpdateDialog::InsertUpdate(std::string name, std::string newVersion)
	{
		TV_INSERTSTRUCT insert;
		std::cout << "name=" << name << "\nversion= " << newVersion << std::endl;
		std::string text = name + ": " + newVersion;
		std::wstring textW = KrollUtils::UTF8ToWide(text);
		
		insert.hParent = NULL;
		insert.hInsertAfter = TVI_ROOT;
		insert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		insert.item.pszText = (wchar_t*) textW.c_str();
		insert.item.iImage = 0;
		insert.item.iSelectedImage = 0;
		return (HTREEITEM) SendMessage(updateList, TVM_INSERTITEM, 0, (LPARAM)&insert);
	}
	
	void UpdateDialog::Initialize(HWND window)
	{
		this->installButton = GetDlgItem(window, IDC_UPDATEINSTALL);
		this->cancelButton = GetDlgItem(window, IDC_UPDATECANCEL);
		this->updateList = GetDlgItem(window, IDC_UPDATELIST);
		
		LOGFONT newFontStruct;
		HFONT currentFont = (HFONT) SendMessage(updateList, WM_GETFONT, (WPARAM)0, (LPARAM)0);
		GetObject(currentFont, sizeof newFontStruct, &newFontStruct);
		newFontStruct.lfWeight = FW_BOLD;
		newFontStruct.lfHeight = 16;
		HFONT newFont = CreateFontIndirect(&newFontStruct);
		SendMessage(updateList, WM_SETFONT, (WPARAM)newFont, LPARAM(0));
		
		HIMAGELIST imageList = ImageList_Create(32,32,ILC_COLOR32,1,10);
		HBITMAP updateBitmap = LoadBitmap(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_UPDATE));
		ImageList_Add(imageList, updateBitmap, NULL);
		DeleteObject(updateBitmap);
		
		SendMessage(updateList, TVM_SETIMAGELIST, 0, (LPARAM)imageList);
		
		for (int i = 0; i < jobs.size(); i++) {
			if (jobs[i]->isUpdate) {
				InsertUpdate(jobs[i]->name, jobs[i]->version);
			}
		}
		
		Dialog::Initialize(window);
	}
	
	void UpdateDialog::ButtonClicked(int id)
	{
		if (id == IDC_UPDATEINSTALL) {
			doInstall = true;
		}
		
		this->Destroy();
	}
}

