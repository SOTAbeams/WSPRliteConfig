#ifndef ui_Box_WSPRSettings_h
#define ui_Box_WSPRSettings_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "ui/Ctl_BandSelect.hpp"
#include "ui/Ctl_OutputPowerSelect.hpp"
#include "ui/Ctl_ReportPowerSelect.hpp"

#include <wx/gbsizer.h>

class Box_WSPRSettings : public wxPanel
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
	wxTextCtrl *ctl_cwId_callsign;
	std::string statsUrl;
	wxTextCtrl *ctl_statsUrl;
	wxButton *ctl_statsOpen;
	wxStaticText *ctl_statsMsg;
	wxBoxSizer *wsprStatsSizer;

	wxGridBagSizer *wsprSizer;
	wxWindow *formParent;

	void addCtl(wxWindow *window, wxGBPosition pos, wxSizerFlags szFlags);
	void addFormRow(int row, wxWindow *label, wxWindow *field);
	void addFormRow(int row, wxString label, wxWindow *field);
public:
	Box_WSPRSettings(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel_);
	virtual ~Box_WSPRSettings();

	void updateStatsLink();
	void updateTxFreqText();
	void OnBandChanged(wxCommandEvent& event);
	void OnCallsignChanged(wxCommandEvent& event);
	void OnBtnStats(wxCommandEvent& event);
	void EnableCtls(bool status);

	void getFields(DeviceConfig &cfg);
	void setFields(const DeviceConfig &cfg);
	bool validate();
};


#endif
