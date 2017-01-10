#include "Ctl_BandSelect.hpp"
#include <random>

bool Ctl_BandSelect::Band::containsFreq(uint32_t f)
{
	// WSPR bands are 200Hz wide, and centred on centreFreq
	return (f>=centreFreq-100 && f<=centreFreq+100);
}

Ctl_BandSelect::Band::Band(wxString txt_, uint32_t centreFreq_, WsprBand bandId_) :
txt(txt_), centreFreq(centreFreq_), bandId(bandId_)
{}

Ctl_BandSelect::Ctl_BandSelect(wxWindow *parent, wxWindowID id) :
wxComboBox(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY | wxCB_DROPDOWN)
{
	bands.push_back(Band(_("20m / 14 MHz"), 14097100ULL, WsprBand::Band_20m));
	bands.push_back(Band(_("30m / 10 MHz"), 10140200ULL, WsprBand::Band_30m));
	bands.push_back(Band(_("40m / 7 MHz"), 7040100ULL, WsprBand::Band_40m));
	// 60m unsupported for now, since there is disagreement over band allocation and WSPR frequency between countries
	//bands.push_back(Band(_("60m / 5.3 MHz"), 5288700ULL, WsprBand::Band_60m));
	bands.push_back(Band(_("80m / 3.6 MHz"), 3594100ULL, WsprBand::Band_80m));
	bands.push_back(Band(_("160m / 1.8 MHz"), 1838100ULL, WsprBand::Band_160m));

	for (Band &b: bands)
	{
		Append(b.txt, (void*)&b);
	}
}

void Ctl_BandSelect::setFreq(uint64_t f)
{
	freq = f;
	for (size_t i=0; i<bands.size(); i++)
	{
		if (bands[i].containsFreq(f))
		{
			SetSelection(i);
			return;
		}
	}
	SetSelection(wxNOT_FOUND);
}

Ctl_BandSelect::Band *Ctl_BandSelect::getSelectedBandInfo()
{
	int i = GetSelection();
	if (i==wxNOT_FOUND)
		return &bands[0];
	return (Band*)GetClientData(i);
}

uint64_t Ctl_BandSelect::getFreq()
{
	if (!getSelectedBandInfo()->containsFreq(freq))
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		int bandSize = 100;
		std::uniform_int_distribution<> randDist(-bandSize/2, bandSize/2);
		freq = getSelectedBandInfo()->centreFreq + randDist(gen);
	}
	return freq;
}

WsprBand Ctl_BandSelect::getBandId()
{
	return getSelectedBandInfo()->bandId;
}

