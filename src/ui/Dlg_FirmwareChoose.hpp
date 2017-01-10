#ifndef ui_Dlg_FirmwareChoose_h
#define ui_Dlg_FirmwareChoose_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <wx/spinctrl.h>
#include "common/Device.hpp"
#include "common/HexFile.hpp"

class Dlg_FirmwareChoose : public wxDialog
{
protected:
	std::shared_ptr<DeviceModel> deviceModel;
	wxTextCtrl *ctl_newFirmwarePath;
	wxStaticText *msg_newFirmwareVersion, *msg_currFirmwareVersion;

public:
	static std::string lastPath;

	HexFile newFirmware;
	bool newFirmwareValid;

	void onBtn_open(wxCommandEvent& event);
	void fw_load(std::string fwPath);
	void fw_updateText();
	Dlg_FirmwareChoose(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel_);
	virtual ~Dlg_FirmwareChoose();

};

#endif
