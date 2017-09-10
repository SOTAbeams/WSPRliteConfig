#include "Dlg_FirmwareChoose.hpp"
#include <wx/gbsizer.h>

std::string Dlg_FirmwareChoose::lastPath = "";

void Dlg_FirmwareChoose::onBtn_open(wxCommandEvent &event)
{
	wxFileDialog openFileDialog(this, _("Open firmware file"), "", "", "Hex File (*.hex)|*.hex", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetPath(lastPath);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;
	lastPath = openFileDialog.GetPath();
	fw_load(std::string(openFileDialog.GetPath()));
}

void Dlg_FirmwareChoose::fw_load(std::string fwPath)
{
	HexFile hexFile;
	hexFile.load(fwPath);
	if (hexFile.getVersion().isValid())
	{
		if (hexFile.getVersion().supports_device(deviceModel->info.deviceVersion))
		{
			newFirmware = hexFile;
			newFirmwareValid = true;
			fw_updateText();
		}
		else
		{
			newFirmwareValid = false;
			fw_updateText();
			wxMessageBox(_("That firmware does not support the currently connected device."), _("Error"), wxOK | wxICON_ERROR );
		}
	}
	else
	{
		newFirmwareValid = false;
		fw_updateText();
		wxMessageBox(_("The selected firmware file is invalid."), _("Error"), wxOK | wxICON_ERROR );
	}
}

void Dlg_FirmwareChoose::fw_updateText()
{
	if (newFirmwareValid)
	{
		std::string txt = newFirmware.getVersion().toString();
		if (newFirmware.getVersion() < deviceModel->info.firmwareVersion)
			txt += " (" + _("older") + ")";
		msg_newFirmwareVersion->SetLabelText(txt);
		ctl_newFirmwarePath->SetValue(newFirmware.hexFilename);
	}
	else
	{
		msg_newFirmwareVersion->SetLabelText(_("none"));
		ctl_newFirmwarePath->SetValue(_("none"));
	}
	msg_currFirmwareVersion->SetLabelText(deviceModel->info.firmwareVersion.toString());
}

Dlg_FirmwareChoose::Dlg_FirmwareChoose(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel_)
	: wxDialog(parent, wxID_ANY, _("Select new firmware")),
	  deviceModel(deviceModel_),
	  newFirmwareValid(false)
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel *formPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxGridBagSizer* formSizer = new wxGridBagSizer(0, 10);
	mainSizer->Add(formPanel, 0, wxEXPAND | wxALL, 10);

	int row = 0;
	formSizer->Add(new wxStaticText(formPanel, wxID_ANY, _("Select firmware update file:")), wxGBPosition(row,0), wxGBSpan(1,2), wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 1);
	row++;

	wxBoxSizer *openFirmwareSizer = new wxBoxSizer(wxHORIZONTAL);
	ctl_newFirmwarePath = new wxTextCtrl(formPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	ctl_newFirmwarePath->SetMinSize(wxSize(300, -1));
	openFirmwareSizer->Add(ctl_newFirmwarePath, 1, wxALIGN_CENTER_VERTICAL | wxALL);
	wxButton *btn_openFirmware = new wxButton(formPanel, wxID_ANY, _("Open"));
	btn_openFirmware->Bind(wxEVT_BUTTON, &Dlg_FirmwareChoose::onBtn_open, this);
	openFirmwareSizer->Add(btn_openFirmware, 0, wxALIGN_CENTER_VERTICAL | wxALL);

	formSizer->Add(openFirmwareSizer, wxGBPosition(row,0), wxGBSpan(1,2), wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 1);
	row++;

	formSizer->Add(10,10, wxGBPosition(row,0), wxGBSpan(1,2), 0, 1);
	row++;

	formSizer->Add(new wxStaticText(formPanel, wxID_ANY, _("Current version:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 0);
	msg_currFirmwareVersion = new wxStaticText(formPanel, wxID_ANY, wxEmptyString);
	formSizer->Add(msg_currFirmwareVersion, wxGBPosition(row,1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 1);
	row++;

	formSizer->Add(new wxStaticText(formPanel, wxID_ANY, _("Update version:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 0);
	msg_newFirmwareVersion = new wxStaticText(formPanel, wxID_ANY, wxEmptyString);
	formSizer->Add(msg_newFirmwareVersion, wxGBPosition(row,1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 1);
	row++;

	mainSizer->AddStretchSpacer(1);
	mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Expand().DoubleBorder());

	formPanel->SetSizerAndFit(formSizer);
	SetSizerAndFit(mainSizer);
	fw_updateText();

	SetMinSize(GetBestSize());
	Fit();

	if (lastPath!="")
	{
		HexFile hexFile;
		hexFile.load(lastPath);
		if (hexFile.getVersion().isValid() && hexFile.getVersion().supports_device(deviceModel->info.deviceVersion))
		{
			newFirmware = hexFile;
			newFirmwareValid = true;
			fw_updateText();
		}
	}
}

Dlg_FirmwareChoose::~Dlg_FirmwareChoose()
{}
