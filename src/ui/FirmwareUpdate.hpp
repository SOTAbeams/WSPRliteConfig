#ifndef ui_FirmwareUpdate_h
#define ui_FirmwareUpdate_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "common/Device.hpp"
#include "common/HexFile.hpp"

bool FirmwareUpdate_runUi(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel, HexFile hexFile);

#endif
