#include "Box_WSPRSettings.hpp"

#include "ui/MaidenheadValidator.hpp"
#include "common/dxplorer.hpp"
#include "common/StrUtil.hpp"

#include <wx/gbsizer.h>
#include <wx/hyperlink.h>
#include <wx/valnum.h>

Box_WSPRSettings::Box_WSPRSettings(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel_) :
	wxStaticBoxSizer(wxVERTICAL, parent, _("WSPR settings")),
	deviceModel(deviceModel_)
{
	wxGridBagSizer* wsprSizer = new wxGridBagSizer();
	Add(wsprSizer, 0, wxEXPAND | wxALL, 10);

	int row = 0;
	txt_callsign = new wxTextCtrl(parent, wxID_ANY);
	txt_callsign->Bind(wxEVT_TEXT, &Box_WSPRSettings::OnCallsignChanged, this);
	wsprSizer->Add(new wxStaticText(parent, wxID_ANY, _("Callsign:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	wsprSizer->Add(txt_callsign, wxGBPosition(row,1), wxDefaultSpan, wxALL | wxEXPAND, 1);
	row++;
	txt_locator = new wxTextCtrl(parent, wxID_ANY);
	txt_locator->SetValidator(MaidenheadValidator());
	wsprSizer->Add(new wxStaticText(parent, wxID_ANY, _("Locator:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	wsprSizer->Add(txt_locator, wxGBPosition(row,1), wxDefaultSpan, wxALL | wxEXPAND, 1);
	row++;
	wsprSizer->Add(new wxStaticText(parent, wxID_ANY, _("Note: the WSPR protocol limits the locator\nto 4 characters (e.g. JN29)")), wxGBPosition(row,1), wxGBSpan(1,1), wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_LEFT, 1);
	row++;
	wsprSizer->Add(new wxHyperlinkCtrl(parent, wxID_ANY, _("Find my locator"), "http://qthlocator.free.fr/"), wxGBPosition(row,1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_LEFT, 1);
	row++;
	ctl_band = new Ctl_BandSelect(parent, wxID_ANY);
	ctl_band->Bind(wxEVT_COMBOBOX, &Box_WSPRSettings::OnBandChanged, this);
	wsprSizer->Add(new wxStaticText(parent, wxID_ANY, _("Band:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	wsprSizer->Add(ctl_band, wxGBPosition(row,1), wxDefaultSpan, wxALL | wxEXPAND, 1);
	row++;
	wsprSizer->Add(new wxStaticText(parent, wxID_ANY, _("Transmit frequency:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	msg_freq = new wxStaticText(parent, wxID_ANY, wxEmptyString);
	wsprSizer->Add(msg_freq, wxGBPosition(row,1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_LEFT, 1);
	row++;
	msg_band = new wxStaticText(parent, wxID_ANY, _("External lowpass filter required"));
	wsprSizer->Add(msg_band, wxGBPosition(row,1), wxGBSpan(1,1), wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_LEFT, 1);
	row++;
	/*txt_outPower = new wxTextCtrl(mainPanel, wxID_ANY);
	wsprSizer->Add(new wxStaticText(mainPanel, wxID_ANY, _("Output power / mW:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	wsprSizer->Add(txt_outPower, wxGBPosition(row,1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 1);
	row++;*/
	ctl_outputPowerSelect = new Ctl_OutputPowerSelect(parent, wxID_ANY);
	wsprSizer->Add(new wxStaticText(parent, wxID_ANY, _("WSPRlite output power:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	wsprSizer->Add(ctl_outputPowerSelect, wxGBPosition(row,1), wxDefaultSpan, wxALL | wxEXPAND, 1);
	row++;
	ctl_reportPowerSelect = new Ctl_ReportPowerSelect(parent, wxID_ANY);
	wsprSizer->Add(new wxStaticText(parent, wxID_ANY, _("Reported transmit power:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	wsprSizer->Add(ctl_reportPowerSelect, wxGBPosition(row,1), wxDefaultSpan, wxALL | wxEXPAND, 1);
	row++;

	ctl_txRate = new wxTextCtrl(parent, wxID_ANY);
	wxIntegerValidator<int> validator_txRate(&value_txRate, wxNUM_VAL_DEFAULT);
	validator_txRate.SetRange(1,50);
	ctl_txRate->SetValidator(validator_txRate);
	wsprSizer->Add(new wxStaticText(parent, wxID_ANY, _("Repeat rate (%):")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	wsprSizer->Add(ctl_txRate, wxGBPosition(row,1), wxDefaultSpan, wxALL | wxEXPAND, 1);
	//wsprSizer->Add(new wxStaticText(configPanel, wxID_ANY, _("Below 50% recommended")), wxGBPosition(row,2), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_LEFT, 1);
	row++;
	ctl_maxDuration = new wxTextCtrl(parent, wxID_ANY);
	wxFloatingPointValidator<float> validator_maxDuration(3, &value_maxDuration, wxNUM_VAL_NO_TRAILING_ZEROES);
	validator_maxDuration.SetRange(0,30);
	ctl_maxDuration->SetValidator(validator_maxDuration);
	wsprSizer->Add(new wxStaticText(parent, wxID_ANY, _("Max run time (days):")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	wsprSizer->Add(ctl_maxDuration, wxGBPosition(row,1), wxDefaultSpan, wxALL | wxEXPAND, 1);
	row++;

	wsprSizer->Add(new wxStaticText(parent, wxID_ANY, _("Statistics:")), wxGBPosition(row,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT, 1);
	wsprStatsSizer = new wxBoxSizer(wxHORIZONTAL);
	ctl_statsUrl = new wxTextCtrl(parent, wxID_ANY);
	ctl_statsUrl->SetEditable(false);
	ctl_statsUrl->SetDefaultStyle(wxTextAttr(*wxBLUE));
	wsprStatsSizer->Add(ctl_statsUrl, wxSizerFlags().Expand().Proportion(1));
	ctl_statsOpen = new wxButton(parent, wxID_ANY, _("Open in browser"));
	ctl_statsOpen->Bind(wxEVT_BUTTON, &Box_WSPRSettings::OnBtnStats, this);
	wsprStatsSizer->Add(ctl_statsOpen, wxSizerFlags());
	ctl_statsMsg = new wxStaticText(parent, wxID_ANY, wxEmptyString);
	ctl_statsMsg->Hide();
	wsprStatsSizer->Add(ctl_statsMsg, 1, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_RIGHT);
	wsprSizer->Add(wsprStatsSizer, wxGBPosition(row,1), wxDefaultSpan, wxALL | wxEXPAND, 1);
	row++;

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
}

void Box_WSPRSettings::getFields(DeviceConfig &cfg)
{
	cfg.callsign = std::string(txt_callsign->GetValue().Upper());
	cfg.locator = std::string(txt_locator->GetValue());
	cfg.band = ctl_band->getBandId();
	cfg.outputPower_dBm = ctl_outputPowerSelect->getdBm();
	cfg.reportedPower_dBm = ctl_reportPowerSelect->getdBm();
	if (cfg.reportedPower_dBm==-1)
		cfg.reportedPower_dBm = cfg.outputPower_dBm;
	cfg.paBias = ctl_outputPowerSelect->getpaBias(cfg.band);
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
