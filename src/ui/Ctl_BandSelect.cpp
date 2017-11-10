#include "Ctl_BandSelect.hpp"
#include "common/RandomFreqGenerator.hpp"

Ctl_BandSelect::Ctl_BandSelect(wxWindow *parent, wxWindowID id, std::shared_ptr<DeviceModel> deviceModel_) :
wxComboBox(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY | wxCB_DROPDOWN),
deviceModel(deviceModel_), freq(14097100ULL)
{
}

void Ctl_BandSelect::initChoices()
{
	bands.clear();
	if (!deviceModel->info.deviceVersion.has_20mFilter())
	{
		// Higher frequencies are only supported by devices without the built in filter
		addBand(WsprBand::Band_6m);
		addBand(WsprBand::Band_10m);
		addBand(WsprBand::Band_12m);
		addBand(WsprBand::Band_15m);
		addBand(WsprBand::Band_17m);
	}

	addBand(WsprBand::Band_20m);
	addBand(WsprBand::Band_30m);
	addBand(WsprBand::Band_40m);
	// Note: to enable 60m, uncomment the relevant line in src/common/WsprBand.cpp
	addBand(WsprBand::Band_60m);
	addBand(WsprBand::Band_80m);
	addBand(WsprBand::Band_160m);
	addBand(WsprBand::Band_630m);

	Clear();
	for (WsprBandInfo* b: bands)
	{
		std::string txt = std::to_string(b->approxWl) + "m / " + b->toString_shortMHz();
		Append(txt, (void*)b);
	}
	setFreq(freq);
}

void Ctl_BandSelect::setFreq(uint64_t f)
{
	freq = f;
	for (size_t i=0; i<bands.size(); i++)
	{
		if (bands[i]->containsFreq(f))
		{
			SetSelection(i);
			return;
		}
	}
	SetSelection(wxNOT_FOUND);
}

WsprBandInfo *Ctl_BandSelect::getSelectedBandInfo()
{
	int i = GetSelection();
	if (i==wxNOT_FOUND)
	{
		if (bands.size())
			return bands[0];
		else
			return nullptr;
	}
	return (WsprBandInfo*)GetClientData(i);
}

void Ctl_BandSelect::genFreq()
{
	if (getSelectedBandInfo()!=nullptr)
	{
		freq = randomFreqGenerator.generate(deviceModel, getBandId());
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

void Ctl_BandSelect::addBand(WsprBandInfo *band)
{
	if (band!=nullptr) {
		bands.push_back(band);
	}
}

void Ctl_BandSelect::addBand(WsprBand bandId)
{
	addBand(WsprBandInfo::find(bandId));
}

bool Ctl_BandSelect::isValidFreq(uint64_t f) const
{
	for (const WsprBandInfo *band : bands)
	{
		if (band->containsFreq(f))
		{
			return true;
		}
	}
	return false;
}

