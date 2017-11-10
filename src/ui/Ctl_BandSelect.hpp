#ifndef ui_Ctl_BandSelect_h
#define ui_Ctl_BandSelect_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "common/Device.hpp"
#include "common/WsprBand.hpp"

#include <memory>

class Ctl_BandSelect : public wxComboBox
{
public:
	std::shared_ptr<DeviceModel> deviceModel;
	uint64_t freq;
	std::vector<WsprBandInfo*> bands;
	Ctl_BandSelect(wxWindow *parent, wxWindowID id, std::shared_ptr<DeviceModel> deviceModel_);

	void initChoices();

	// Selects the band to which the given frequency belongs
	void setFreq(uint64_t f);

	WsprBandInfo* getSelectedBandInfo();
	void genFreq();
	uint64_t getFreq();
	WsprBand getBandId();

	void addBand(WsprBandInfo* band);
	void addBand(WsprBand bandId);
	bool isValidFreq(uint64_t f) const;
};


#endif
