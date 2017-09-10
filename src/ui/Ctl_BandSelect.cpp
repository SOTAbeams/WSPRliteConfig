#include "Ctl_BandSelect.hpp"
#include <random>
#include <chrono>

bool Ctl_BandSelect::Band::containsFreq(uint32_t f)
{
	// WSPR bands are 200Hz wide, and centred on centreFreq
	return (f>=centreFreq-100 && f<=centreFreq+100);
}

Ctl_BandSelect::Band::Band(wxString txt_, uint32_t centreFreq_, WsprBand bandId_) :
txt(txt_), centreFreq(centreFreq_), bandId(bandId_)
{}

Ctl_BandSelect::Ctl_BandSelect(wxWindow *parent, wxWindowID id) :
wxComboBox(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY | wxCB_DROPDOWN),
rng(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
freq(14097100ULL)
{
}

void Ctl_BandSelect::setDeviceVersion(DeviceVersion deviceVersion)
{
	bands.clear();
	if (!deviceVersion.has_20mFilter())
	{
		// Higher frequencies are only supported by devices without the built in filter
		bands.push_back(Band(_("6m / 50 MHz"), 50294500ULL, WsprBand::Band_6m));
		bands.push_back(Band(_("10m / 28 MHz"), 28126100ULL, WsprBand::Band_10m));
		bands.push_back(Band(_("12m / 25 MHz"), 24926100ULL, WsprBand::Band_12m));
		bands.push_back(Band(_("15m / 21 MHz"), 21096100ULL, WsprBand::Band_15m));
		bands.push_back(Band(_("17m / 18 MHz"), 18106100ULL, WsprBand::Band_17m));
	}

	bands.push_back(Band(_("20m / 14 MHz"), 14097100ULL, WsprBand::Band_20m));
	bands.push_back(Band(_("30m / 10 MHz"), 10140200ULL, WsprBand::Band_30m));
	bands.push_back(Band(_("40m / 7 MHz"), 7040100ULL, WsprBand::Band_40m));
	// 60m unsupported for now, since there is disagreement over band allocation and WSPR frequency between countries
	//bands.push_back(Band(_("60m / 5.3 MHz"), 5288700ULL, WsprBand::Band_60m));
	bands.push_back(Band(_("80m / 3.6 MHz"), 3594100ULL, WsprBand::Band_80m));
	bands.push_back(Band(_("160m / 1.8 MHz"), 1838100ULL, WsprBand::Band_160m));
	bands.push_back(Band(_("630m / 0.47 MHz"), 475700ULL, WsprBand::Band_630m));

	Clear();
	for (Band &b: bands)
	{
		Append(b.txt, (void*)&b);
	}
	setFreq(freq);
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
	{
		if (bands.size())
			return &bands[0];
		else
			return nullptr;
	}
	return (Band*)GetClientData(i);
}

void Ctl_BandSelect::genFreq()
{
	if (getSelectedBandInfo()!=nullptr)
	{
		int bandSize = 100;
		std::uniform_int_distribution<> randDist(-bandSize/2, bandSize/2);
		freq = getSelectedBandInfo()->centreFreq + randDist(rng);
	}
}

uint64_t Ctl_BandSelect::getFreq()
{
	if (getSelectedBandInfo()==nullptr)
		return 0;

	if (!getSelectedBandInfo()->containsFreq(freq))
	{
		genFreq();
	}
	return freq;
}

WsprBand Ctl_BandSelect::getBandId()
{
	if (getSelectedBandInfo()!=nullptr)
		return getSelectedBandInfo()->bandId;
	else
		return WsprBand::Band_20m;
}

