#ifndef ui_Box_WSPRSettings_h
#define ui_Box_WSPRSettings_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "ui/Ctl_BandSelect.hpp"
#include "ui/Ctl_OutputPowerSelect.hpp"
#include "ui/Ctl_ReportPowerSelect.hpp"

class Box_WSPRSettings : public wxStaticBoxSizer
{
protected:
	std::shared_ptr<DeviceModel> deviceModel;

	wxTextCtrl *txt_callsign, *txt_locator, *ctl_txRate, *ctl_maxDuration;
	int value_txRate;
	float value_maxDuration;
	Ctl_BandSelect *ctl_band;
	Ctl_OutputPowerSelect *ctl_outputPowerSelect;
	Ctl_ReportPowerSelect *ctl_reportPowerSelect;
	wxStaticText *msg_band, *msg_freq;
	std::string statsUrl;
	wxTextCtrl *ctl_statsUrl;
	wxButton *ctl_statsOpen;
	wxStaticText *ctl_statsMsg;
	wxBoxSizer *wsprStatsSizer;

public:
	Box_WSPRSettings(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel_);
	virtual ~Box_WSPRSettings();

	void updateStatsLink();
	void updateTxFreqText();
	void OnBandChanged(wxCommandEvent& event);
	void OnCallsignChanged(wxCommandEvent& event);
	void OnBtnStats(wxCommandEvent& event);
	void Enable(bool status);

	void getFields(DeviceConfig &cfg);
	void setFields(const DeviceConfig &cfg);
	bool validate();
};


#endif
