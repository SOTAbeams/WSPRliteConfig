#ifndef ui_Ctl_ReportPowerSelect_h
#define ui_Ctl_ReportPowerSelect_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <vector>

class Ctl_ReportPowerSelect : public wxComboBox
{
public:
	class PowerLevel
	{
	public:
		wxString txt;
		int8_t dBm;
		PowerLevel(int8_t dBm_);
		PowerLevel(int8_t dBm_, wxString txt_);
	};

	std::vector<PowerLevel> powerLevels;
	Ctl_ReportPowerSelect(wxWindow *parent, wxWindowID id);

	void setdBm(int8_t dBm);
	PowerLevel* getSelectedPowerInfo();
	int8_t getdBm();
};


#endif
