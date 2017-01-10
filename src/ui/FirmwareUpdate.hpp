#ifndef ui_FirmwareUpdate_h
#define ui_FirmwareUpdate_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <wx/progdlg.h>

#include "common/Device.hpp"
#include "common/HexFile.hpp"
#include "common/Task_FirmwareUpdate.hpp"

class FirmwareUpdateProgressDlg : public wxProgressDialog
{
protected:
	wxWindow *parent;
	std::shared_ptr<DeviceModel> deviceModel;

	Task_FirmwareUpdate task;
	std::thread actionThread;

public:
	bool success = false;
	FirmwareUpdateProgressDlg(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel_, HexFile hexFile);
	virtual ~FirmwareUpdateProgressDlg();
};

#endif
