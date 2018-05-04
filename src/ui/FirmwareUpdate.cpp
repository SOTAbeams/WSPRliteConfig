#include "FirmwareUpdate.hpp"
#include <wx/progdlg.h>
#include "common/Task_FirmwareUpdate.hpp"
#include <mutex>

class FirmwareUpdateUiState
{
public:
	std::mutex mtx;
	bool finished = false;
	bool success = false;
	int progressValue = 0;
	wxString progressMsg;
	wxString errorMsg;
};

bool FirmwareUpdate_runUi(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel, HexFile hexFile)
{
	const int progressScale = 10000;
	Task_FirmwareUpdate task(deviceModel);
	std::thread actionThread;
	FirmwareUpdateUiState state;
	wxProgressDialog progressDlg("WSPRlite Firmware Update", "", progressScale, parent, wxPD_AUTO_HIDE | wxPD_APP_MODAL);

	task.onError.add([&state](std::string msg){
		std::lock_guard<std::mutex> lk(state.mtx);
		state.success = false;
		state.finished = true;
		state.progressValue = progressScale;
		state.progressMsg = "Error";
		state.errorMsg = msg;
	});
	task.onProgress.add([&state](float progress, std::string msg){
		std::lock_guard<std::mutex> lk(state.mtx);
		state.progressValue = progress*progressScale;
		state.progressMsg = msg;
	});
	task.onSuccess.add([&state](){
		std::lock_guard<std::mutex> lk(state.mtx);
		state.success = true;
		state.finished = true;
		state.progressValue = progressScale;
		state.progressMsg = "Finished";
	});
	task.fwHex = hexFile;

	actionThread = std::thread([&task](){
		task.runSync();
	});

	while (1)
	{
		{
			std::lock_guard<std::mutex> lk(state.mtx);
			progressDlg.Update(state.progressValue, state.progressMsg);
			if (state.finished)
				break;
		}
		wxMilliSleep(50);
	}

	if (actionThread.joinable())
		actionThread.join();

	{
		std::lock_guard<std::mutex> lk(state.mtx);
		progressDlg.Update(progressScale, state.progressMsg);
		if (!state.success)
		{
			wxMessageBox("Firmware update failed.\n" + state.errorMsg, _("Error"), wxOK | wxICON_ERROR );
			return false;
		}
		else
		{
			return true;
		}
	}

}
