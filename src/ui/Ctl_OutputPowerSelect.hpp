#ifndef ui_Ctl_OutputPowerSelect_h
#define ui_Ctl_OutputPowerSelect_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <vector>
#include "common/Device.hpp"

class Ctl_OutputPowerSelect : public wxComboBox
{
public:
	class PowerLevel
	{
	public:
		wxString txt;
		int8_t dBm;
		uint16_t paBias_14MHz, paBias_10MHz, paBias_other;
		PowerLevel(int8_t dBm_, uint16_t paBias_14MHz_, uint16_t paBias_10MHz_, uint16_t paBias_other_);
	};

	std::vector<PowerLevel> powerLevels;
	Ctl_OutputPowerSelect(wxWindow *parent, wxWindowID id);

	void setdBm(int8_t dBm);
	PowerLevel* getSelectedPowerInfo();
	int8_t getdBm();
	uint16_t getpaBias(WsprBand bandId);
};


#endif
