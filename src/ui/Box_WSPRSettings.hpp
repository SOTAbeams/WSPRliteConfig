#ifndef ui_Box_WSPRSettings_h
#define ui_Box_WSPRSettings_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "ui/Ctl_BandSelect.hpp"
#include "ui/Ctl_BiasSelect.hpp"
#include "ui/Ctl_OutputPowerSelect.hpp"
#include "ui/Ctl_ReportPowerSelect.hpp"

#include <wx/gbsizer.h>

class Box_WSPRSettings : public wxPanel
{
protected:
	std::shared_ptr<DeviceModel> deviceModel;

	wxTextCtrl *txt_callsign, *txt_locator, *ctl_txRate, *ctl_maxDuration, *ctl_freq;
	int value_txRate;
	int value_freq;
	float value_maxDuration;
	Ctl_BandSelect *ctl_band;
	Ctl_BiasSelect *ctl_biasSelect;
	Ctl_OutputPowerSelect *ctl_outputPowerSelect;
	Ctl_ReportPowerSelect *ctl_reportPowerSelect;
	wxStaticText *msg_band, *msg_freq, *msg_biasSelect;
	wxCheckBox *ctl_cwId_enable;
	wxTextCtrl *ctl_cwId_callsign;
	wxCheckBox *ctl_forceExtendedWspr;
	wxStaticText *msg_wsprMsgTypes;
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
	void updateAvailableOutputPowers();
	void updateMaxDurationValidator();
	void cwIdCtls_updateStatus();
	void checkFreqOverride();
	void updateBiasSelectText();
	void updateWsprMsgTypesText();
	void extendedWsprCtls_updateStatus();
	void OnBandChanged(wxCommandEvent& event);
	void OnBiasSelectChanged(wxCommandEvent& event);
	void OnCallsignChanged(wxCommandEvent& event);
	void OnLocatorChanged(wxCommandEvent& event);
	void OnBtnStats(wxCommandEvent& event);
	void OnCWIDEnableChange(wxCommandEvent& event);
	void OnForceExtendedWsprChange(wxCommandEvent& event);
	void OnFreqUnfocus(wxFocusEvent& event);
	void EnableCtls(bool status);
	void onCtlsChanged();

	void getFields(DeviceConfig &cfg);
	void setFields(const DeviceConfig &cfg);
	bool validate();
};


#endif
