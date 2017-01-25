#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "ui/PortChoose.hpp"
#include "ui/Dlg_TestRF.hpp"
#include "ui/Dlg_FirmwareChoose.hpp"
#include "ui/FirmwareUpdate.hpp"
#include "ui/Box_WSPRSettings.hpp"

#include "common/Device.hpp"
#include "common/Task_Connect.hpp"
#include "common/Task_StatusCheck.hpp"
#include "common/Task_WSPRLoad.hpp"
#include "common/Task_WSPRSave.hpp"

#include <memory>
#include <cctype>
#include <sstream>
#include <iomanip>

const FirmwareVersion minFirmwareVersionRecommended(1,0,5,20170119);


class WSPRConfigApp: public wxApp
{
public:
	bool OnInit() override;
	int OnExit() override;
	void OnUnhandledException() override;
};
class WSPRConfigFrame: public wxFrame
{
public:
	WSPRConfigFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	std::shared_ptr<DeviceModel> deviceModel;
private:
	wxButton *btn_load, *btn_save;
	wxStaticText *msg_devStatus;

	Box_WSPRSettings *wsprSettingsBox;

	TimerFn portChoose_timer;
	TimerFn devStatusCheck_timer;
	PortChooseList *portChoose_list;
	wxButton *btn_connect, *btn_firmwareUpdate, *btn_testRF=nullptr;
	wxStaticText *msg_firmwareVersion;

	wxPanel* panel_config;

	int failedStatusCheckCount = 0;

	void deviceLoad();
	void deviceSave();
	void deviceConnect();
	void connectStartComm(bool firmwareUpdateAttempted=false);
	void updateVersionText();
	void onDisconnect();
	void WSPRCtlsEnabled(bool status);
	void startStatusUpdate();
	void doFirmwareUpdate();

	void OnExit(wxCommandEvent& event);
	void LoadWSPR(wxCommandEvent& event);
	void SaveWSPR(wxCommandEvent& event);
	void OnBtnConnect(wxCommandEvent& event);
	void OnBtnFirmwareUpdate(wxCommandEvent& event);
	void OnBtnTestRF(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};
enum
{
	ID_SaveWSPR = wxID_HIGHEST+1,
	ID_Btn_Connect,
	ID_Btn_FirmwareUpdate,
	ID_Btn_TestRF,
};
wxBEGIN_EVENT_TABLE(WSPRConfigFrame, wxFrame)
EVT_MENU(wxID_EXIT,  WSPRConfigFrame::OnExit)
EVT_BUTTON(ID_Btn_Connect, WSPRConfigFrame::OnBtnConnect)
EVT_BUTTON(ID_Btn_FirmwareUpdate, WSPRConfigFrame::OnBtnFirmwareUpdate)
EVT_BUTTON(ID_SaveWSPR, WSPRConfigFrame::SaveWSPR)
EVT_BUTTON(ID_Btn_TestRF, WSPRConfigFrame::OnBtnTestRF)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(WSPRConfigApp);

bool WSPRConfigApp::OnInit()
{
	WSPRConfigFrame *frame = new WSPRConfigFrame("WSPRlite settings - v1.0.5", wxPoint(50, 50), wxSize(450, 340) );
	frame->Show( true );
	return true;
}
int WSPRConfigApp::OnExit()
{
	return 0;
}
void WSPRConfigApp::OnUnhandledException()
{
	std::string error;
	try {
		throw; // Rethrow the current exception.
	} catch (const std::exception& e) {
		error = e.what();
	} catch ( ... ) {
		error = "unknown error.";
	}
	wxMessageOutputBest().Printf("Unexpected exception has occurred: %s, the program will terminate.", error.c_str());
}

WSPRConfigFrame::WSPRConfigFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	deviceModel = std::make_shared<DeviceModel>();
	deviceModel->info.auth.id = 0;

	CreateStatusBar();
	SetStatusText("");
#if defined(_WIN32) || defined(_WIN64)
	SetIcon(wxIcon("IDI_APPICON"));
#endif

	wxPanel* mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	panel_config = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer *configSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(panel_config, wxSizerFlags().Expand());

	wxStaticBoxSizer* deviceBox = new wxStaticBoxSizer(wxVERTICAL, panel_config, _("Hardware"));
	configSizer->Add(deviceBox, wxSizerFlags().Expand().Border(wxALL, 6));
	wxBoxSizer* deviceSizer = new wxBoxSizer(wxVERTICAL);
	deviceBox->Add(deviceSizer, wxSizerFlags().Expand().Border(wxALL, 10));

	deviceSizer->Add(new wxStaticText(panel_config, wxID_ANY, _("Select serial port to use:")));

	wxBoxSizer* portChooseSizer = new wxBoxSizer(wxHORIZONTAL);
	portChoose_list = new PortChooseList(panel_config);
	portChoose_list->SetMinClientSize(wxSize(300,-1));
	portChooseSizer->Add(portChoose_list, wxSizerFlags().Expand().Proportion(1));
	btn_connect = new wxButton(panel_config, ID_Btn_Connect, _("Connect"));
	portChooseSizer->Add(btn_connect, wxSizerFlags().Expand());
	deviceSizer->Add(portChooseSizer, wxSizerFlags().Expand());

	deviceSizer->AddSpacer(10);

	msg_firmwareVersion = new wxStaticText(panel_config, wxID_ANY, wxEmptyString);
	deviceSizer->Add(msg_firmwareVersion,  wxSizerFlags().Expand().Left());
	msg_devStatus = new wxStaticText(panel_config, wxID_ANY, wxEmptyString);
	deviceSizer->Add(msg_devStatus, wxSizerFlags().Expand().Left().ReserveSpaceEvenIfHidden());

	wxBoxSizer* deviceBtnsSizer = new wxBoxSizer(wxHORIZONTAL);
	deviceBtnsSizer->AddStretchSpacer(1);
	btn_firmwareUpdate = new wxButton(panel_config, ID_Btn_FirmwareUpdate, _("Update firmware"));
	deviceBtnsSizer->Add(btn_firmwareUpdate, wxSizerFlags().Align(wxALIGN_CENTRE_VERTICAL));

	// This is handy for testing frequency accuracy and power output, but will not be useful for most users so is commented out:
	/*btn_testRF = new wxButton(panel_config, ID_Btn_TestRF, _("RF output test"));
	deviceBtnsSizer->Add(btn_testRF, wxSizerFlags().Border(wxALL, 6).Align(wxALIGN_CENTRE_VERTICAL));*/

	deviceBtnsSizer->AddSpacer(10);
	btn_save = new wxButton(panel_config, ID_SaveWSPR, _("Save WSPR settings"));
	deviceBtnsSizer->Add(btn_save, wxSizerFlags().Border(wxALL, 6).Align(wxALIGN_CENTRE_VERTICAL));
	deviceSizer->Add(deviceBtnsSizer, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 6));// TODO: proportion?

	portChoose_list->preferredDesc = "WSPRlite";
	portChoose_list->UpdatePorts();
	portChoose_list->SetFocus();

	portChoose_timer.callback = [this](){
		portChoose_list->UpdatePorts();
	};

	wsprSettingsBox = new Box_WSPRSettings(panel_config, deviceModel);
	configSizer->Add(wsprSettingsBox, wxSizerFlags().Expand().Border(wxALL, 6));

	panel_config->SetSizerAndFit(configSizer);

	mainPanel->SetSizerAndFit(mainSizer);

	WSPRCtlsEnabled(false);
	updateVersionText();
	SetMinSize(GetBestSize());
	Fit();

	devStatusCheck_timer.callback = [this](){
		startStatusUpdate();
	};

	portChoose_timer.Start(2000);
	devStatusCheck_timer.Start(1000);
}

void WSPRConfigFrame::deviceLoad()
{
	SetStatusText(_("Loading..."));

	std::shared_ptr<Task_WSPRLoad> task = std::make_shared<Task_WSPRLoad>(deviceModel);
	task->onError.add([this](std::string msg){
		GetEventHandler()->CallAfter([this, msg](){
			wxMessageBox(msg, _("Error"), wxOK | wxICON_ERROR );
			onDisconnect();
		});
	});
	task->onSuccess.add([this](){
		GetEventHandler()->CallAfter([this](){
			SetStatusText("");
			btn_connect->Enable(true);
			wsprSettingsBox->setFields(deviceModel->config);
			WSPRCtlsEnabled(true);
			if (deviceModel->info.firmwareVersion < minFirmwareVersionRecommended)
			{
				wxMessageBox(_("A firmware update has been released which fixes a bug in your WSPRlite. Before adjusting settings, please update the firmware.\n\nVisit http://dxplorer.net/wsprlite/ to download the update and for instructions."), _("Firmware update required"), wxOK | wxICON_WARNING );
			}
			else
			{
				startStatusUpdate();
			}
			updateVersionText();
		});
	});
	deviceModel->taskRunner.enqueue(task);
}

void WSPRConfigFrame::deviceSave()
{
	if (!deviceModel->conn)
		return;

	wsprSettingsBox->validate();

	std::shared_ptr<Task_WSPRSave> task = std::make_shared<Task_WSPRSave>(deviceModel);
	task->newCfg = deviceModel->config;
	try
	{
		wsprSettingsBox->getFields(task->newCfg);
	}
	catch (std::exception const &e)
	{
		wxMessageBox(e.what(), _("Error"), wxOK | wxICON_ERROR );
		return;
	}

	if (!DeviceConfig::isValidCallsign(task->newCfg.callsign))
	{
		wxMessageBox(_("Callsign is invalid. A callsign must consist of 1 or 2 letters/numbers, then a number, then up to 3 letters."), _("Invalid callsign"), wxOK | wxICON_ERROR );
		return;
	}

	SetStatusText(_("Saving..."));
	btn_save->Disable();
	task->onError.add([this](std::string msg){
		GetEventHandler()->CallAfter([this, msg](){
			wxMessageBox(msg, _("Error"), wxOK | wxICON_ERROR );
			onDisconnect();
		});
	});
	task->onSuccess.add([this](){
		GetEventHandler()->CallAfter([this](){
			SetStatusText("");
			wsprSettingsBox->updateStatsLink();
			btn_save->Enable();
		});
	});
	deviceModel->taskRunner.enqueue(task);
}


void WSPRConfigFrame::deviceConnect()
{
	SetStatusText(_("Connecting..."));
	btn_connect->Enable(false);
	try
	{
		if (deviceModel->conn)
			deviceModel->conn->disconnect();
		deviceModel->conn = std::make_shared<DeviceComm::Connection>(portChoose_list->GetChosenPort());
		if (deviceModel->conn)
		{
			connectStartComm();
		}
	}
	catch (std::exception const &e)
	{
		wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR );
		onDisconnect();
	}
}

void WSPRConfigFrame::connectStartComm(bool firmwareUpdateAttempted)
{
	std::shared_ptr<Task_Connect> task = std::make_shared<Task_Connect>(deviceModel);
	task->onError.add([this](std::string msg){
		GetEventHandler()->CallAfter([this, msg](){
			wxMessageBox(_("Unable to connect.\n") + msg, _("Error"), wxOK | wxICON_ERROR );
			onDisconnect();
		});
	});
	task->onSuccess.add([this, firmwareUpdateAttempted](){
		GetEventHandler()->CallAfter([this, firmwareUpdateAttempted](){
			if (deviceModel->bootloaderState>=1)
			{
				if (!firmwareUpdateAttempted)
				{
					wxMessageBox(_("No valid firmware loaded - please update the firmware"), _("Error"), wxOK | wxICON_INFORMATION );
					doFirmwareUpdate();
				}
				else
				{
					wxMessageBox(_("Unable to connect - no valid firmware loaded"), _("Error"), wxOK | wxICON_ERROR );
					onDisconnect();
				}
			}
			else
			{
				deviceLoad();
			}
		});
	});
	deviceModel->taskRunner.enqueue(task);
}

void WSPRConfigFrame::updateVersionText()
{
	if (deviceModel->conn==nullptr)
	{
		msg_firmwareVersion->SetLabelText(_("Firmware version:") + " none (disconnected)");
	}
	else
	{
		msg_firmwareVersion->SetLabelText(_("Firmware version:") + " " + deviceModel->info.firmwareVersion.toString());
	}
}

void WSPRConfigFrame::onDisconnect()
{
	deviceModel->conn = nullptr;
	updateVersionText();
	SetStatusText("");
	msg_devStatus->SetLabelText(_("Disconnected"));
	btn_connect->Enable(true);
	WSPRCtlsEnabled(false);
}

void WSPRConfigFrame::WSPRCtlsEnabled(bool status)
{
	wsprSettingsBox->Enable(status);
	btn_save->Enable(status);
	if (btn_testRF)
		btn_testRF->Enable(status);
	if (btn_firmwareUpdate)
		btn_firmwareUpdate->Enable(status);
}

void WSPRConfigFrame::startStatusUpdate()
{
	if (!deviceModel || !deviceModel->conn || !deviceModel->conn->isValid())
	{
		msg_devStatus->Hide();
		msg_devStatus->SetLabelText(_("Status: disconnected"));
		return;
	}
	std::shared_ptr<Task_StatusCheck> task = std::make_shared<Task_StatusCheck>(deviceModel);
	task->onError.add([this](std::string msg){
		GetEventHandler()->CallAfter([this](){
			failedStatusCheckCount++;
			if (failedStatusCheckCount>3)
				onDisconnect();
		});
	});
	task->onSuccess.add([this](){
		GetEventHandler()->CallAfter([this](){
			failedStatusCheckCount = 0;
			if (!deviceModel->info.firmwareVersion.supports_deviceMode())
			{
				msg_devStatus->Hide();
				return;
			}
			msg_devStatus->Show();
			switch (deviceModel->mode)
			{
			case DeviceMode::Test_ConstantTx:
				msg_devStatus->SetLabelText(_("Status: test mode - RF output"));
				break;
			case DeviceMode::HardwareFail:
				msg_devStatus->SetLabelText(_("Error: hardware failure, please contact SOTAbeams"));
				break;
			case DeviceMode::FactoryInvalid:
				msg_devStatus->SetLabelText(_("Error: missing factory config, please contact SOTAbeams"));
				break;
			case DeviceMode::WSPR_Invalid:
				msg_devStatus->SetLabelText(_("Status: WSPR settings not yet configured"));
				break;
			case DeviceMode::WSPR_Pending:
				msg_devStatus->SetLabelText(_("Status: WSPR mode, waiting to start"));
				break;
			case DeviceMode::WSPR_Active:
			{
				wxString txt = _("Status: WSPR mode active");
				if (deviceModel->submode.wspr == DeviceModeSub_WSPR::idle)
					txt += wxString(", ") + _("waiting for next transmit timeslot");
				else if (deviceModel->submode.wspr == DeviceModeSub_WSPR::tx)
					txt += wxString(", ") + _("transmitting");
				msg_devStatus->SetLabelText(txt);
				break;
			}
			default:
				msg_devStatus->SetLabelText(_("Unknown status"));
				break;
			}
		});
	});
	deviceModel->taskRunner.enqueue(task);
}

void WSPRConfigFrame::doFirmwareUpdate()
{
	Dlg_FirmwareChoose fwChooseDlg(this, deviceModel);
	if (fwChooseDlg.ShowModal() == wxID_CANCEL)
		return;

	if (fwChooseDlg.newFirmwareValid)
	{
		if (fwChooseDlg.newFirmware.getVersion() < deviceModel->info.firmwareVersion)
		{
			if (wxMessageBox(_("The selected firmware is older than the firmware currently on the WSPRlite. Are you sure you want to continue?"), _("Confirm firmware downgrade"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION ) != wxYES)
			{
				return;
			}
		}
		FirmwareUpdateProgressDlg dlg(this, deviceModel, fwChooseDlg.newFirmware);
		dlg.ShowModal();
		deviceConnect();
	}
}

void WSPRConfigFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void WSPRConfigFrame::LoadWSPR(wxCommandEvent &event)
{
	deviceLoad();
}

void WSPRConfigFrame::SaveWSPR(wxCommandEvent &event)
{
	deviceSave();
}

void WSPRConfigFrame::OnBtnConnect(wxCommandEvent &event)
{
	deviceConnect();
}

void WSPRConfigFrame::OnBtnFirmwareUpdate(wxCommandEvent &event)
{
	doFirmwareUpdate();
}

void WSPRConfigFrame::OnBtnTestRF(wxCommandEvent &event)
{
	Dlg_TestRF dlg(this, deviceModel);
	dlg.ShowModal();
}

