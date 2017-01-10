#include "FirmwareUpdate.hpp"
#include "common/crc.hpp"

const static int progressScale = 10000;

FirmwareUpdateProgressDlg::FirmwareUpdateProgressDlg(wxWindow *parent_, std::shared_ptr<DeviceModel> deviceModel_, HexFile hexFile)
: wxProgressDialog("WSPRlite Firmware Update", "", progressScale, parent_), parent(parent_),
deviceModel(deviceModel_), task(deviceModel_)
{
	task.onError.add([this](std::string msg){
		GetEventHandler()->CallAfter([this, msg](){
			wxMessageBox("Firmware update failed.\n" + msg, _("Error"), wxOK | wxICON_ERROR );
			Update(progressScale, "Error");
			Close();
		});
	});
	task.onProgress.add([this](float progress, std::string msg){
		GetEventHandler()->CallAfter([this, progress, msg](){
			Update(progress*progressScale, msg);
		});
	});
	task.onSuccess.add([this](){
		GetEventHandler()->CallAfter([this](){
			Update(progressScale, "Finished");
			success = true;
			Close();
		});
	});
	task.fwHex = hexFile;

	actionThread = std::thread([this](){
		task.runSync();
	});
}

FirmwareUpdateProgressDlg::~FirmwareUpdateProgressDlg()
{
	if (actionThread.joinable())
		actionThread.join();
}
