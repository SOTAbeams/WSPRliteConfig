#include "Box_WSPRSettings.hpp"

#include "ui/MaidenheadValidator.hpp"
#include "common/dxplorer.hpp"
#include "common/StrUtil.hpp"

#include <wx/hyperlink.h>
#include <wx/valnum.h>

void Box_WSPRSettings::addCtl(wxWindow *window, wxGBPosition pos, wxSizerFlags szFlags)
{
	wsprSizer->Add(window, pos, wxDefaultSpan, szFlags.GetFlags(), szFlags.GetBorderInPixels());
}

void Box_WSPRSettings::addFormRow(int row, wxWindow *label, wxWindow *field)
{
	if (label)
		addCtl(label, wxGBPosition(row,0), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT).Border(wxALL, 0).ReserveSpaceEvenIfHidden());
	if (field)
		addCtl(field, wxGBPosition(row,1), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT).Expand().Border(wxALL, 0).ReserveSpaceEvenIfHidden());
}

void Box_WSPRSettings::addFormRow(int row, wxString label, wxWindow *field)
{
	addFormRow(row, new wxStaticText(formParent, wxID_ANY, label), field);
}

Box_WSPRSettings::Box_WSPRSettings(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel_) :
	wxStaticBoxSizer(wxVERTICAL, parent, _("WSPR settings")),
	deviceModel(deviceModel_)
{
	wsprSizer = new wxGridBagSizer();
	Add(wsprSizer, wxSizerFlags().Expand().Border(wxALL, 10));
	formParent = parent;

	int row = 0;
	txt_callsign = new wxTextCtrl(formParent, wxID_ANY);
	txt_callsign->Bind(wxEVT_TEXT, &Box_WSPRSettings::OnCallsignChanged, this);
	addFormRow(row++, _("WSPR ident:"), txt_callsign);

	txt_locator = new wxTextCtrl(formParent, wxID_ANY);
	txt_locator->SetValidator(MaidenheadValidator());
	addFormRow(row++, _("Locator:"), txt_locator);

	addFormRow(row++, nullptr, new wxStaticText(formParent, wxID_ANY, _("Note: the WSPR protocol limits the locator\nto 4 characters (e.g. JN29)")));
	addCtl(new wxHyperlinkCtrl(formParent, wxID_ANY, _("Find my locator"), "http://qthlocator.free.fr/"), wxGBPosition(row++, 1), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT).Border(wxALL, 0));

	ctl_band = new Ctl_BandSelect(formParent, wxID_ANY);
	ctl_band->Bind(wxEVT_COMBOBOX, &Box_WSPRSettings::OnBandChanged, this);
	addFormRow(row++, _("Band:"), ctl_band);

	msg_freq = new wxStaticText(formParent, wxID_ANY, wxEmptyString);
	addFormRow(row++, _("Transmit frequency:"), msg_freq);

	msg_band = new wxStaticText(formParent, wxID_ANY, _("External lowpass filter required"));
	addFormRow(row++, nullptr, msg_band);

	ctl_outputPowerSelect = new Ctl_OutputPowerSelect(formParent, wxID_ANY);
	addFormRow(row++, _("WSPRlite output power:"), ctl_outputPowerSelect);

	ctl_reportPowerSelect = new Ctl_ReportPowerSelect(formParent, wxID_ANY);
	addFormRow(row++, _("Reported transmit power:"), ctl_reportPowerSelect);

	ctl_txRate = new wxTextCtrl(formParent, wxID_ANY);
	wxIntegerValidator<int> validator_txRate(&value_txRate, wxNUM_VAL_DEFAULT);
	validator_txRate.SetRange(1,50);
	ctl_txRate->SetValidator(validator_txRate);
	addFormRow(row++, _("Repeat rate (%):"), ctl_txRate);

	ctl_maxDuration = new wxTextCtrl(formParent, wxID_ANY);
	wxFloatingPointValidator<float> validator_maxDuration(3, &value_maxDuration, wxNUM_VAL_NO_TRAILING_ZEROES);
	validator_maxDuration.SetRange(0,30);
	ctl_maxDuration->SetValidator(validator_maxDuration);
	addFormRow(row++, _("Max run time (days):"), ctl_maxDuration);

	wsprSizer->Add(new wxStaticText(formParent, wxID_ANY, _("Statistics:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	wsprStatsSizer = new wxBoxSizer(wxHORIZONTAL);
	ctl_statsUrl = new wxTextCtrl(formParent, wxID_ANY);
	ctl_statsUrl->SetEditable(false);
	ctl_statsUrl->SetDefaultStyle(wxTextAttr(*wxBLUE));
	wsprStatsSizer->Add(ctl_statsUrl, wxSizerFlags().Expand().Proportion(1));
	ctl_statsOpen = new wxButton(formParent, wxID_ANY, _("Open in browser"));
	ctl_statsOpen->Bind(wxEVT_BUTTON, &Box_WSPRSettings::OnBtnStats, this);
	wsprStatsSizer->Add(ctl_statsOpen, wxSizerFlags());
	ctl_statsMsg = new wxStaticText(formParent, wxID_ANY, wxEmptyString);
	ctl_statsMsg->Hide();
	wsprStatsSizer->Add(ctl_statsMsg, 1, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT);
	wsprSizer->Add(wsprStatsSizer, wxGBPosition(row,1), wxDefaultSpan, wxALL | wxEXPAND, 1);
	row++;

	ctl_cwId_callsign = new wxTextCtrl(formParent, wxID_ANY);
	ctl_cwId_callsign->SetMaxLength(12);
	addFormRow(row++, _("CW callsign (only use\n if absolutely necessary):"), ctl_cwId_callsign);

	wsprSizer->AddGrowableCol(1,1);

	updateStatsLink();
	updateTxFreqText();
}

Box_WSPRSettings::~Box_WSPRSettings()
{}


void Box_WSPRSettings::OnBandChanged(wxCommandEvent& event)
{
	updateTxFreqText();
}

void Box_WSPRSettings::OnCallsignChanged(wxCommandEvent &event)
{
	updateStatsLink();
}

void Box_WSPRSettings::Enable(bool status)
{
	txt_callsign->Enable(status);
	txt_locator->Enable(status);
	ctl_band->Enable(status);
	ctl_outputPowerSelect->Enable(status);
	ctl_reportPowerSelect->Enable(status);
	ctl_maxDuration->Enable(status);
	ctl_txRate->Enable(status);
	msg_freq->Show(status);
	ctl_cwId_callsign->Enable(status && deviceModel->info.firmwareVersion.supports_cwId());
}

void Box_WSPRSettings::getFields(DeviceConfig &cfg)
{

	std::string newCallsign = std::string(txt_callsign->GetValue().Upper());
	if (cfg.callsign!=newCallsign)
		cfg.changeCounter++;

	if (cfg.changeCounter<=1)
	{
		// This is the first time configuring this particular WSPRlite, so a
		// new random tx frequency was generated when setFields was called.
		// Increment changeCounter to indicate that this has been done.
		cfg.changeCounter = 2;
	}

	cfg.callsign = newCallsign;
	cfg.locator = std::string(txt_locator->GetValue());
	cfg.band = ctl_band->getBandId();
	cfg.outputPower_dBm = ctl_outputPowerSelect->getdBm();
	cfg.reportedPower_dBm = ctl_reportPowerSelect->getdBm();
	if (cfg.reportedPower_dBm==-1)
		cfg.reportedPower_dBm = cfg.outputPower_dBm;
	cfg.paBias = ctl_outputPowerSelect->getpaBias(ctl_band->getFreq());
	try
	{
		cfg.maxRuntime = StrUtil::stringToDouble(std::string(ctl_maxDuration->GetValue()))*24*3600;
	}
	catch (std::exception const &e)
	{
		throw std::invalid_argument("Invalid value for max run time");
	}
	try
	{
		cfg.transmitPercent = StrUtil::stringToInt(std::string(ctl_txRate->GetValue()));
	}
	catch (std::exception const &e)
	{
		throw std::invalid_argument("Invalid value for repeat rate");
	}
	cfg.transmitFreq = ctl_band->getFreq();

	cfg.cwId_callsign = ctl_cwId_callsign->GetValue().Upper();
	if (cfg.cwId_callsign!="")
		cfg.cwId_freq = ctl_band->getSelectedBandInfo()->centreFreq - 150;
	else
		cfg.cwId_freq = 0;
}

void Box_WSPRSettings::setFields(const DeviceConfig &cfg)
{
	txt_callsign->SetValue(cfg.callsign);
	txt_locator->SetValue(cfg.locator);
	ctl_outputPowerSelect->setdBm(cfg.outputPower_dBm);
	if (cfg.outputPower_dBm==cfg.reportedPower_dBm)
		ctl_reportPowerSelect->setdBm(-1);
	else
		ctl_reportPowerSelect->setdBm(cfg.reportedPower_dBm);
	ctl_maxDuration->SetValue(StrUtil::doubleToString((double)cfg.maxRuntime/(24*3600)));
	ctl_txRate->SetValue(std::to_string(cfg.transmitPercent));
	ctl_band->setFreq(cfg.transmitFreq);
	ctl_cwId_callsign->SetValue(cfg.cwId_callsign);
	if (cfg.changeCounter<=1)
	{
		ctl_band->genFreq();
	}
	ctl_cwId_callsign->Enable(deviceModel->info.firmwareVersion.supports_cwId());
	updateTxFreqText();
	updateStatsLink();
}

bool Box_WSPRSettings::validate()
{
	if (txt_locator->GetValidator() && !txt_locator->GetValidator()->Validate(txt_locator))
	{
		return false;
	}
	return true;
}


void Box_WSPRSettings::updateStatsLink()
{
	bool unsavedChanges = (std::string(txt_callsign->GetValue())!=deviceModel->config.callsign);

	std::string url = "http://dxplorer.net/";
	if (deviceModel->config.callsign!="" && deviceModel->info.auth.id)
	{
		std::string key = DXplorer::generateKey(deviceModel->info.auth.id, deviceModel->info.auth.secret, deviceModel->config.changeCounter, deviceModel->config.callsign);
		url += "wspr/tx/";
		// TODO: ideally, callsign would be URL-encoded here, but the current restrictions on valid WSPR callsigns mean that not doing encoding is OK at the moment
		url += "?callsign=" + deviceModel->config.callsign + "&key=" + key;
	}
	statsUrl = url;

	if (unsavedChanges || (!deviceModel->info.auth.id && deviceModel->conn))
	{
		if (unsavedChanges)
		{
			ctl_statsMsg->SetLabelText(_("Save settings to device to get new stats link"));
		}
		else
		{
			ctl_statsMsg->SetLabelText(_("Error - missing ID. Contact SOTAbeams for help."));
		}
		ctl_statsMsg->Show();
		ctl_statsUrl->Hide();
		ctl_statsOpen->Hide();
	}
	else
	{
		ctl_statsUrl->SetValue(url);
		ctl_statsMsg->Hide();
		ctl_statsUrl->Show();
		ctl_statsOpen->Show();
	}
	wsprStatsSizer->Layout();
}

void Box_WSPRSettings::updateTxFreqText()
{
	WsprBand bandId = ctl_band->getBandId();
	if (bandId==WsprBand::Band_20m || bandId==WsprBand::Band_30m)
		msg_band->Hide();
	else
		msg_band->Show();
	msg_freq->SetLabelText(std::to_string(ctl_band->getFreq()) + "Hz  " + _("(picked randomly within band)"));
	Layout();
}

void Box_WSPRSettings::OnBtnStats(wxCommandEvent &event)
{
	wxLaunchDefaultBrowser(statsUrl);
}
