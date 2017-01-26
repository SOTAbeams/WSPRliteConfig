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
	std::vector<int8_t> powerLevels;
	Ctl_OutputPowerSelect(wxWindow *parent, wxWindowID id);

	void setdBm(int8_t dBm);
	int8_t getdBm();
	uint16_t getpaBias(uint64_t freq);
};


#endif
