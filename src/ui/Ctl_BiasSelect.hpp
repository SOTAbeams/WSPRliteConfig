#ifndef ui_Ctl_BiasSelect_h
#define ui_Ctl_BiasSelect_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <vector>
#include "common/Device.hpp"

class Ctl_BiasSelect : public wxComboBox
{
public:
	std::shared_ptr<DeviceModel> deviceModel;
	std::vector<PaBiasSource> choices;
	Ctl_BiasSelect(wxWindow *parent, wxWindowID id, std::shared_ptr<DeviceModel> deviceModel_);

	void initChoices();
	void set(PaBiasSource newChoice);
	PaBiasSource get();
};


#endif
