#ifndef ui_Ctl_BandSelect_h
#define ui_Ctl_BandSelect_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <random>

#include "common/Device.hpp"

class Ctl_BandSelect : public wxComboBox
{
protected:
	std::mt19937 rng;

public:
	class Band
	{
	public:
		wxString txt;
		uint64_t centreFreq;
		WsprBand bandId;
		Band(wxString txt_, uint32_t centreFreq_, WsprBand bandId_);
		bool containsFreq(uint32_t f);
	};

	uint64_t freq;
	std::vector<Band> bands;
	Ctl_BandSelect(wxWindow *parent, wxWindowID id);

	// Selects the band to which the given frequency belongs
	void setFreq(uint64_t f);

	Band* getSelectedBandInfo();
	void genFreq();
	uint64_t getFreq();
	WsprBand getBandId();
};


#endif
