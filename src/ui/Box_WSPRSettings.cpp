#include "Box_WSPRSettings.hpp"

#include "ui/MaidenheadValidator.hpp"
#include "common/dxplorer.hpp"
#include "common/StrUtil.hpp"
#include "common/PaBias.hpp"
#include "common/WsprCallsign.hpp"
#include "common/WsprMsgTypes.hpp"

#include <wx/hyperlink.h>
#include <wx/valnum.h>
#include <cstdio>

void Box_WSPRSettings::addCtl(wxWindow *window, wxGBPosition pos, wxSizerFlags szFlags)
{
	wsprSizer->Add(window, pos, wxDefaultSpan, szFlags.GetFlags(), szFlags.GetBorderInPixels());
}

void Box_WSPRSettings::addFormRow(int row, wxWindow *label, wxWindow *field)
{
	if (label)
		addCtl(label, wxGBPosition(row,0), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT).Border(wxALL, 1));
	if (field)
		addCtl(field, wxGBPosition(row,1), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT).Expand().Border(wxALL, 1));
}

void Box_WSPRSettings::addFormRow(int row, wxString label, wxWindow *field)
{
	addFormRow(row, new wxStaticText(formParent, wxID_ANY, label), field);
}

Box_WSPRSettings::Box_WSPRSettings(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel_) :
	wxPanel(parent),
	deviceModel(deviceModel_)
{
	wsprSizer = new wxGridBagSizer();
	wsprSizer->SetEmptyCellSize(wxSize(0,0));
	//Add(wsprSizer, wxSizerFlags().Expand().Border(wxALL, 10));
	formParent = this;

	int row = 0;

	// Min width for controls column
	wsprSizer->Add(350, 1, wxGBPosition(row++,1));

	{
		// Title
		wxSizerFlags szFlags = wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
		wxStaticText *ctl = new wxStaticText(formParent, wxID_ANY, _("WSPR settings"));
		wxFont font = ctl->GetFont();
		font.SetWeight(wxFONTWEIGHT_BOLD);
		ctl->SetFont(font);
		wsprSizer->Add(ctl, wxGBPosition(row,0), wxGBSpan(1,2), szFlags.GetFlags(), szFlags.GetBorderInPixels());
	}
	row++;

	wsprSizer->Add(1, 10, wxGBPosition(row++,0), wxGBSpan(1,2));// Spacer below title

	txt_callsign = new wxTextCtrl(formParent, wxID_ANY);
	txt_callsign->Bind(wxEVT_TEXT, &Box_WSPRSettings::OnCallsignChanged, this);
	addFormRow(row++, _("WSPR callsign:"), txt_callsign);

	ctl_cwId_enable = new wxCheckBox(formParent, wxID_ANY, "");
	ctl_cwId_enable->Bind(wxEVT_CHECKBOX, &Box_WSPRSettings::OnCWIDEnableChange, this);
	addFormRow(row++, _("CW callsign:"), ctl_cwId_enable);

	ctl_cwId_callsign = new wxTextCtrl(formParent, wxID_ANY);
	ctl_cwId_callsign->SetMaxLength(12);
	addFormRow(row++, nullptr, ctl_cwId_callsign);

	wsprSizer->Add(1, 10, wxGBPosition(row++,0), wxGBSpan(1,2));// spacer

	txt_locator = new wxTextCtrl(formParent, wxID_ANY);
	txt_locator->SetValidator(MaidenheadValidator());
	txt_locator->Bind(wxEVT_TEXT, &Box_WSPRSettings::OnLocatorChanged, this);
	addFormRow(row++, _("Locator:"), txt_locator);

	addCtl(new wxHyperlinkCtrl(formParent, wxID_ANY, _("Find my locator"), "http://qthlocator.free.fr/"), wxGBPosition(row++, 1), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT).Border(wxALL, 0));

	ctl_forceExtendedWspr = new wxCheckBox(formParent, wxID_ANY, "");
	ctl_forceExtendedWspr->Bind(wxEVT_CHECKBOX, &Box_WSPRSettings::OnForceExtendedWsprChange, this);
	addFormRow(row++, nullptr, ctl_forceExtendedWspr);
	msg_wsprMsgTypes = new wxStaticText(formParent, wxID_ANY, "");
	addFormRow(row++, nullptr, msg_wsprMsgTypes);

	ctl_extendedWsprInfoLink = new wxHyperlinkCtrl(formParent, wxID_ANY, _("Click here to read about why extended WSPR should\nbe avoided if possible"), "http://dxplorer.net/wspr/msgtypes.html#whynot23");
	addCtl(ctl_extendedWsprInfoLink, wxGBPosition(row++, 1), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT).Border(wxALL, 0));

	ctl_band = new Ctl_BandSelect(formParent, wxID_ANY, deviceModel);
	ctl_band->Bind(wxEVT_COMBOBOX, &Box_WSPRSettings::OnBandChanged, this);
	addFormRow(row++, _("Band:"), ctl_band);

	msg_freq = new wxStaticText(formParent, wxID_ANY, wxEmptyString);
	ctl_freq = new wxTextCtrl(formParent, wxID_ANY);
	ctl_freq->Bind(wxEVT_KILL_FOCUS, &Box_WSPRSettings::OnFreqUnfocus, this);
	wxIntegerValidator<int> validator_freq(&value_freq, wxNUM_VAL_DEFAULT);
	validator_freq.SetRange(0,52000000);
	ctl_freq->SetValidator(validator_freq);

	wxSizer *freqCtlsSizer = new wxBoxSizer(wxVERTICAL);
	freqCtlsSizer->Add(msg_freq, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT).Border(wxALL, 1));
	freqCtlsSizer->Add(ctl_freq, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT).Expand().Border(wxALL, 1));
	addFormRow(row, _("Transmit frequency / Hz:"), nullptr);
	wsprSizer->Add(freqCtlsSizer, wxGBPosition(row,1), wxDefaultSpan, wxALL | wxEXPAND, 1);
	ctl_freq->Hide();
	row++;

	msg_band = new wxStaticText(formParent, wxID_ANY, _("External lowpass filter required"));
	addFormRow(row++, nullptr, msg_band);

	ctl_outputPowerSelect = new Ctl_OutputPowerSelect(formParent, wxID_ANY);
	addFormRow(row++, _("WSPRlite output power:"), ctl_outputPowerSelect);

	ctl_reportPowerSelect = new Ctl_ReportPowerSelect(formParent, wxID_ANY);
	addFormRow(row++, _("Reported transmit power:"), ctl_reportPowerSelect);

	ctl_biasSelect = new Ctl_BiasSelect(formParent, wxID_ANY, deviceModel);
	ctl_biasSelect->Bind(wxEVT_COMBOBOX, &Box_WSPRSettings::OnBiasSelectChanged, this);
	addFormRow(row++, _("Filter loss compensation:"), ctl_biasSelect);

	msg_biasSelect = new wxStaticText(formParent, wxID_ANY, wxEmptyString);
	addFormRow(row++, nullptr, msg_biasSelect);


	ctl_txRate = new wxTextCtrl(formParent, wxID_ANY);
	wxIntegerValidator<int> validator_txRate(&value_txRate, wxNUM_VAL_DEFAULT);
	validator_txRate.SetRange(1,50);
	ctl_txRate->SetValidator(validator_txRate);
	addFormRow(row++, _("Repeat rate (%):"), ctl_txRate);

	ctl_maxDuration = new wxTextCtrl(formParent, wxID_ANY);
	updateMaxDurationValidator();
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

	// spacer - simple workaround to allow room for hidden controls, since resizing the various parent sizers and controls is a bit more complicated
	wsprSizer->Add(1, 50, wxGBPosition(row++,0), wxGBSpan(1,2));

	wsprSizer->AddGrowableCol(1,1);

	updateStatsLink();
	updateTxFreqText();
	cwIdCtls_updateStatus();
	extendedWsprCtls_updateStatus();
	updateWsprMsgTypesText();

	SetSizerAndFit(wsprSizer);
}

Box_WSPRSettings::~Box_WSPRSettings()
{}


void Box_WSPRSettings::OnBandChanged(wxCommandEvent& event)
{
	if (ctl_band->getBandId()==WsprBand::Band_630m && deviceModel && !deviceModel->info.firmwareVersion.supports_band(WsprBand::Band_630m))
	{
		wxMessageBox(_("Please update your WSPRlite firmware to v1.0.6 or later before using the 630m band"), _("CW callsign"), wxOK | wxICON_WARNING);
		ctl_band->setFreq(deviceModel->config.transmitFreq);
	}
	updateTxFreqText();
	updateAvailableOutputPowers();
	updateBiasSelectText();
}

void Box_WSPRSettings::OnBiasSelectChanged(wxCommandEvent &event)
{
	updateAvailableOutputPowers();
	updateBiasSelectText();
}

void Box_WSPRSettings::OnCallsignChanged(wxCommandEvent &event)
{
	updateWsprMsgTypesText();
	extendedWsprCtls_updateStatus();
	updateStatsLink();
}

void Box_WSPRSettings::OnLocatorChanged(wxCommandEvent &event)
{
	updateWsprMsgTypesText();
	extendedWsprCtls_updateStatus();
}

void Box_WSPRSettings::EnableCtls(bool status)
{
	txt_callsign->Enable(status);
	txt_locator->Enable(status);
	ctl_band->Enable(status);
	ctl_biasSelect->Enable(status);
	ctl_outputPowerSelect->Enable(status);
	ctl_reportPowerSelect->Enable(status);
	ctl_maxDuration->Enable(status);
	ctl_txRate->Enable(status);

	if (status && deviceModel->hasCalibratedOscillator()) {
		// Frequency override is only supported on devices where the oscillator has been calibrated, since on uncalibrated devices the frequency may be significantly different from the set value (may be up to a few tens of Hz, though the typical error is smaller)
		// No point in allowing an override if the WSPRlite cannot guarantee that it will produce the user-supplied value with reasonable accuracy
		ctl_freq->Show(true);
		msg_freq->Show(false);
	} else {
		ctl_freq->Show(false);
		msg_freq->Show(status);
	}
	ctl_freq->Enable(status);

	if (!status)
	{
		ctl_cwId_enable->Enable(status);
		ctl_cwId_callsign->Enable(status);
		ctl_forceExtendedWspr->Enable(status);
	}
	else
	{
		cwIdCtls_updateStatus();
		extendedWsprCtls_updateStatus();
	}
	onCtlsChanged();
}

void Box_WSPRSettings::onCtlsChanged()
{
	wsprSizer->Layout();
	Layout();
	Fit();
	Update();
	Refresh();
}

void Box_WSPRSettings::getFields(DeviceConfig &cfg)
{
	checkFreqOverride();

	std::string newCallsignRaw = txt_callsign->GetValue().ToStdString();
	std::string newCallsign = WsprCallsign::canonicalFormat(newCallsignRaw);
	if (newCallsignRaw!=newCallsign)
	{
		txt_callsign->SetValue(newCallsign);
	}
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
	for (int i=0; i<2 && i<=cfg.locator.length(); i++)
	{
		cfg.locator[i] = std::toupper(cfg.locator[i]);
	}

	cfg.biasSource = ctl_biasSelect->get();

	cfg.outputPower_dBm = ctl_outputPowerSelect->getdBm();
	cfg.reportedPower_dBm = ctl_reportPowerSelect->getdBm();
	if (cfg.reportedPower_dBm==-1)
		cfg.reportedPower_dBm = cfg.outputPower_dBm;
	cfg.paBias = PaBias::get(deviceModel->info, cfg.biasSource, ctl_band->getFreq(), ctl_outputPowerSelect->getdBm());
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
	if (cfg.cwId_callsign!="" && ctl_cwId_enable->GetValue() && ctl_band->getSelectedBandInfo()!=nullptr)
		cfg.cwId_freq = ctl_band->getSelectedBandInfo()->centreFreq - 150;
	else
		cfg.cwId_freq = 0;

	cfg.optionFlags = 0;
	if (ctl_forceExtendedWspr->IsChecked())
		cfg.optionFlags |= DeviceComm::VarFlag::WSPR_option_forceExtended;
}

void Box_WSPRSettings::setFields(const DeviceConfig &cfg)
{
	txt_callsign->SetValue(cfg.callsign);
	txt_locator->SetValue(cfg.locator);
	ctl_biasSelect->initChoices();
	ctl_biasSelect->set(cfg.biasSource);
	ctl_outputPowerSelect->setdBm(cfg.outputPower_dBm);
	if (cfg.outputPower_dBm==cfg.reportedPower_dBm)
		ctl_reportPowerSelect->setdBm(-1);
	else
		ctl_reportPowerSelect->setdBm(cfg.reportedPower_dBm);
	ctl_maxDuration->SetValue(StrUtil::doubleToString((double)cfg.maxRuntime/(24*3600)));
	ctl_txRate->SetValue(std::to_string(cfg.transmitPercent));
	ctl_band->initChoices();
	ctl_band->setFreq(cfg.transmitFreq);
	ctl_cwId_callsign->SetValue(cfg.cwId_callsign);
	ctl_cwId_enable->SetValue(deviceModel->info.firmwareVersion.supports_cwId() && cfg.cwId_freq && cfg.cwId_callsign!="");
	ctl_forceExtendedWspr->SetValue(cfg.optionFlags & DeviceComm::VarFlag::WSPR_option_forceExtended);
	if (cfg.changeCounter<=1)
	{
		ctl_band->genFreq();
	}

	cwIdCtls_updateStatus();
	updateTxFreqText();
	updateStatsLink();
	updateAvailableOutputPowers();
	updateMaxDurationValidator();
	updateBiasSelectText();
	extendedWsprCtls_updateStatus();
	updateWsprMsgTypesText();
	onCtlsChanged();
}

bool Box_WSPRSettings::validate()
{
	if (txt_locator->GetValidator() && !txt_locator->GetValidator()->Validate(txt_locator))
	{
		return false;
	}
	return true;
}

bool Box_WSPRSettings::isLongLocator()
{
	return txt_locator->GetValue().Len()>=6;
}

bool Box_WSPRSettings::isCompoundCallsign()
{
	std::string newCallsignRaw = txt_callsign->GetValue().ToStdString();
	std::string newCallsign = WsprCallsign::canonicalFormat(newCallsignRaw);
	return WsprCallsign::isCompound(newCallsign);
}

bool Box_WSPRSettings::isConnValid()
{
	return (deviceModel && deviceModel->conn && deviceModel->conn->isValid());
}

void Box_WSPRSettings::updateStatsLink()
{
	bool unsavedChanges = (std::string(txt_callsign->GetValue())!=deviceModel->config.callsign);

	std::string url = "http://dxplorer.net/";
	if (deviceModel->config.callsign!="" && deviceModel->info.auth.id)
	{
		std::string key = DXplorer::generateKey(deviceModel->info.auth.id, deviceModel->info.auth.secret, deviceModel->config.changeCounter, deviceModel->config.callsign);
		WsprBandInfo *bandInfo = WsprBandInfo::findByFreq(deviceModel->config.transmitFreq);
		url += "wspr/tx/";
		url += "?callsign=" + StrUtil::url_encode(deviceModel->config.callsign);
		if (bandInfo)
			url += "&band=" + StrUtil::url_encode(std::to_string(bandInfo->getBandCode()));
		url += "&key=" + StrUtil::url_encode(key);
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
	if (deviceModel->info.deviceVersion.has_20mFilter() && (bandId==WsprBand::Band_20m || bandId==WsprBand::Band_30m))
		msg_band->Hide();
	else
		msg_band->Show();

	std::string txFreqString = std::to_string(ctl_band->getFreq());
	msg_freq->SetLabelText(txFreqString + "Hz  " + _("(picked randomly within band)"));
	if (txFreqString != ctl_freq->GetValue()) {
		ctl_freq->ChangeValue(txFreqString);
	}
	onCtlsChanged();
}

void Box_WSPRSettings::updateAvailableOutputPowers()
{
	uint64_t freq = ctl_band->getFreq();
	PaBiasSource biasSource = ctl_biasSelect->get();
	double minPower = PaBias::getMinPower_dBm(deviceModel->info, biasSource, freq);
	double maxPower = PaBias::getMaxPower_dBm(deviceModel->info, biasSource, freq);
	ctl_outputPowerSelect->initChoices(minPower, maxPower);
}

void Box_WSPRSettings::updateMaxDurationValidator()
{
	wxFloatingPointValidator<float> validator_maxDuration(3, &value_maxDuration, wxNUM_VAL_NO_TRAILING_ZEROES);
	validator_maxDuration.SetRange(0, deviceModel->getMaxRuntimeLimit_days());
	ctl_maxDuration->SetValidator(validator_maxDuration);
}

void Box_WSPRSettings::cwIdCtls_updateStatus()
{
	bool cwIdSupported = deviceModel->info.firmwareVersion.supports_cwId();
	bool cwIdStatus = cwIdSupported && ctl_cwId_enable->GetValue();
	bool connValid = isConnValid();

	if (!cwIdSupported)
	{
		ctl_cwId_enable->SetValue(false);
	}
	ctl_cwId_enable->Enable(cwIdSupported && connValid);
	if (cwIdSupported || !connValid)
	{
		ctl_cwId_enable->SetLabelText(_("Enable (only use if absolutely necessary)"));
	}
	else
	{
		ctl_cwId_enable->SetLabelText(_("unsupported - firmware update needed"));
	}

	wsprSizer->Show(ctl_cwId_callsign, cwIdStatus);
	ctl_cwId_enable->Enable(cwIdSupported && connValid);
	ctl_cwId_callsign->Enable(cwIdStatus && connValid);
	onCtlsChanged();
}

void Box_WSPRSettings::checkFreqOverride()
{
	uint64_t newFreq = StrUtil::stringToUint(ctl_freq->GetValue().ToStdString());
	if (newFreq!=ctl_band->getFreq()) {
		if (ctl_band->isValidFreq(newFreq)) {
			ctl_band->setFreq(newFreq);
			updateTxFreqText();
		} else {
			WsprBandInfo *band = ctl_band->getSelectedBandInfo();
			const char *msg = _("Not a valid WSPR frequency. Valid frequencies for this band are between %llu and %llu Hz.");
			char msgf[sizeof(msg)+100];
			std::sprintf(msgf, msg, (unsigned long long)band->getMinFreq(), (unsigned long long)band->getMaxFreq());
			wxMessageBox(msgf, _("Invalid frequency"), wxOK | wxICON_ERROR);

			// Reset to old frequency stored in band control
			updateTxFreqText();
		}
	}
}

void Box_WSPRSettings::updateBiasSelectText()
{
	bool hasData = PaBias::hasData(deviceModel->info, ctl_biasSelect->get(), ctl_band->getFreq());
	if (hasData) {
		msg_biasSelect->Hide();
	} else {
		msg_biasSelect->SetLabelText(_("No data available for this band and filter type.\nDefaulting to no filter compensation."));
		msg_biasSelect->Show();
	}
	onCtlsChanged();
}

void Box_WSPRSettings::updateWsprMsgTypesText()
{
	WsprMsgTypes msgTypes = WsprMsgTypes::None;
	std::string newCallsignRaw = txt_callsign->GetValue().ToStdString();
	std::string newCallsign = WsprCallsign::canonicalFormat(newCallsignRaw);
	bool locatorTooShort = false;
	bool supportsExtendedWspr = isConnValid() && deviceModel->info.firmwareVersion.supports_extendedWspr();
	if (!deviceModel || !deviceModel->info.firmwareVersion.isValidCallsign(newCallsign))
	{
		msgTypes = WsprMsgTypes::None;
	}
	else if (WsprCallsign::isCompound(newCallsign))
	{
		msgTypes = WsprMsgTypes::Type2_3;
	}
	else if (supportsExtendedWspr && ctl_forceExtendedWspr->IsChecked())
	{
		if (!isLongLocator())
		{
			msgTypes = WsprMsgTypes::Type1;
			locatorTooShort = true;
		}
		else
		{
			msgTypes = WsprMsgTypes::Type1_3;
		}
	}
	else
	{
		msgTypes = WsprMsgTypes::Type1;
	}

	std::string txt = "";
	switch (msgTypes)
	{
	case WsprMsgTypes::None:
		txt = _("Callsign is not valid");
		break;
	case WsprMsgTypes::Type1:
		txt = _("Will use standard WSPR (type 1 messages).");
		if (locatorTooShort)
			txt += _("\nTo use extended WSPR, enter a 6 character locator.");
		else if (isLongLocator())
			txt += _("\nThe transmitted locator will be limited to 4 characters.");
		break;
	case WsprMsgTypes::Type2_3:
		txt = _("Will use extended WSPR (type 2 & 3 messages), \ndue to the compound callsign.");
		break;
	case WsprMsgTypes::Type1_3:
		txt = _("Will use extended WSPR (type 1 & 3 messages).");
		break;
	}
	if (msgTypes==WsprMsgTypes::Type2_3 || msgTypes==WsprMsgTypes::Type1_3)
	{
		ctl_extendedWsprInfoLink->Show(true);
	}
	else
	{
		ctl_extendedWsprInfoLink->Show(false);
	}
	if (!supportsExtendedWspr)
		txt = _("Extended WSPR unsupported, update firmware");
	msg_wsprMsgTypes->SetLabelText(txt);
	msg_wsprMsgTypes->Show(isConnValid());
	//msg_wsprMsgTypes->Wrap(txt_callsign->GetSize().GetWidth());
	onCtlsChanged();
}

void Box_WSPRSettings::extendedWsprCtls_updateStatus()
{
	bool connValid = isConnValid();
	bool isSupported = connValid && deviceModel->info.firmwareVersion.supports_extendedWspr();
	ctl_forceExtendedWspr->Enable(isSupported);
	if (!connValid || !isSupported)
	{
		ctl_forceExtendedWspr->Show(false);
	}
	else
	{
		ctl_forceExtendedWspr->SetLabelText("Transmit 6 character locator");
		ctl_forceExtendedWspr->Show(isLongLocator() && !isCompoundCallsign());
	}
	onCtlsChanged();
}

void Box_WSPRSettings::OnBtnStats(wxCommandEvent &event)
{
	wxLaunchDefaultBrowser(statsUrl);
}

void Box_WSPRSettings::OnCWIDEnableChange(wxCommandEvent &event)
{
	if (event.IsChecked())
	{
		const char *msg = _(
			"The CW callsign feature transmits your callsign in morse code at the end of each WSPR message, 50 Hz below the WSPR band.\n\n"
			"Uses:\n"
			"1) To identify your transmission where the standard WSPR protocol will "
			"not suffice (such as a callsign that does not follow the format required by WSPR).\n"
			"2) To meet regulatory requirements in some countries.\n\n"
			"We do not recommend use of CW identification unless it is actually necessary.");
		wxMessageBox(msg, _("CW callsign"), wxOK | wxICON_INFORMATION );
	}
	cwIdCtls_updateStatus();
}

void Box_WSPRSettings::OnForceExtendedWsprChange(wxCommandEvent &event)
{
	updateWsprMsgTypesText();
}

void Box_WSPRSettings::OnFreqUnfocus(wxFocusEvent &event)
{
	checkFreqOverride();
}
