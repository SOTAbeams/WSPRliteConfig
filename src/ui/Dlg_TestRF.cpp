#include "Dlg_TestRF.hpp"
#include <wx/gbsizer.h>
#include <sstream>

enum
{
	ID_Ctl_TestRF_Freq = wxID_HIGHEST+100,
	ID_Ctl_TestRF_Bias,
};

wxBEGIN_EVENT_TABLE(Dlg_TestRF, wxDialog)
EVT_SPINCTRL(ID_Ctl_TestRF_Freq, Dlg_TestRF::handle_spinEvent)
EVT_SPINCTRL(ID_Ctl_TestRF_Bias, Dlg_TestRF::handle_spinEvent)
EVT_TEXT(ID_Ctl_TestRF_Freq, Dlg_TestRF::handle_spinCmdEvent)
EVT_TEXT(ID_Ctl_TestRF_Bias, Dlg_TestRF::handle_spinCmdEvent)
wxEND_EVENT_TABLE()

uint64_t Dlg_TestRF::lastFreq = 14097100ULL;
uint16_t Dlg_TestRF::lastPaBias = 411;

void Dlg_TestRF::handle_spinEvent(wxSpinEvent &event)
{
	writeToDevice();
}

void Dlg_TestRF::handle_spinCmdEvent(wxCommandEvent &event)
{
	writeToDevice();
}

void Dlg_TestRF::writeToDevice()
{
	if (!deviceModel || !deviceModel->conn)
		return;
	
	std::shared_ptr<Task_TestRF> task = std::make_shared<Task_TestRF>(deviceModel);
	task->freq = ctl_freq->GetValue();
	task->paBias = ctl_paBias->GetValue();
	task->onError.add([this](std::string msg){
		GetEventHandler()->CallAfter([this, msg](){
			wxMessageBox(msg, _("Error"), wxOK | wxICON_ERROR );
			Close();
		});
	});
	taskRunner.enqueue(task);

	lastFreq = task->freq;
	lastPaBias = task->paBias;
}

Dlg_TestRF::Dlg_TestRF(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel_)
	: wxDialog(parent, wxID_ANY, "RF output test"),
	  deviceModel(deviceModel_)
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel *formPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxGridBagSizer* formSizer = new wxGridBagSizer();
	mainSizer->Add(formPanel, 0, wxEXPAND | wxALL, 10);

	int row = 0;
	ctl_freq = new wxSpinCtrlDouble(formPanel, ID_Ctl_TestRF_Freq, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxSP_ARROW_KEYS, 2500, 200000000ULL, lastFreq);
	formSizer->Add(new wxStaticText(formPanel, wxID_ANY, _("Frequency / Hz:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	formSizer->Add(ctl_freq, wxGBPosition(row,1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 1);
	row++;
	ctl_paBias = new wxSpinCtrl(formPanel, ID_Ctl_TestRF_Bias, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxSP_ARROW_KEYS, 0, 1000, lastPaBias);
	formSizer->Add(new wxStaticText(formPanel, wxID_ANY, _("PA bias:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	formSizer->Add(ctl_paBias, wxGBPosition(row,1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 1);
	row++;

	mainSizer->AddStretchSpacer(1);
	mainSizer->Add(CreateSeparatedButtonSizer(wxCLOSE), wxSizerFlags().Expand().DoubleBorder());

	formPanel->SetSizerAndFit(formSizer);
	SetSizerAndFit(mainSizer);

	writeToDevice();
}

Dlg_TestRF::~Dlg_TestRF()
{
	DeviceComm::Msg msg(DeviceComm::MsgType::DeviceMode_Set);
	uint16_t newMode = (uint16_t)DeviceMode::WSPR_Pending;
	msg.data.append_int_le<uint16_t>(newMode);
	if (deviceModel && deviceModel->conn)
		deviceModel->conn->enqueue(msg);
}


void Task_TestRF::task()
{
	DeviceComm::Msg msg(DeviceComm::MsgType::DeviceMode_Set);
	uint16_t newMode = (uint16_t)DeviceMode::Test_ConstantTx;
	msg.data.append_int_le<uint16_t>(newMode);
	msg.data.append_int_le<uint64_t>(freq);
	msg.data.append_int_le<uint16_t>(paBias);
	send(msg).assert_ack();
}

Task_TestRF::~Task_TestRF()
{}
