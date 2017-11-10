#include "WsprBand.hpp"

WsprBandInfoStore allBandsInfo;

bool WsprBandInfo::containsFreq(uint32_t f) const
{
	return (f>=getMinFreq() && f<=getMaxFreq());
}

// WSPR bands are 200Hz wide, and centred on centreFreq
uint64_t WsprBandInfo::getMinFreq() const
{
	return centreFreq-100;
}
uint64_t WsprBandInfo::getMaxFreq() const
{
	return centreFreq+100;
}

std::string WsprBandInfo::toString_shortMHz()
{
	char tmp[100];
	if (centreFreq < 1e6) {
		sprintf(tmp, "%.2f MHz", centreFreq / 1e6);
	} else if (centreFreq < 7e6) {
		sprintf(tmp, "%.1f MHz", centreFreq / 1e6);
	} else {
		sprintf(tmp, "%.0f MHz", centreFreq / 1e6);
	}
	return tmp;
}

WsprBandInfo* WsprBandInfo::find(WsprBand bandId)
{
	auto it = allBandsInfo.bands.find(bandId);
	if (it!=allBandsInfo.bands.end())
		return it->second;
	return nullptr;
}

WsprBandInfo::WsprBandInfo(uint32_t approxWl_, WsprBand bandId_, uint64_t centreFreq_) :
approxWl(approxWl_), bandId(bandId_), centreFreq(centreFreq_)
{}


WsprBandInfoStore::WsprBandInfoStore()
{
	add(new WsprBandInfo(6, WsprBand::Band_6m, 50294500ULL));
	add(new WsprBandInfo(10, WsprBand::Band_10m, 28126100ULL));
	add(new WsprBandInfo(12, WsprBand::Band_12m, 24926100ULL));
	add(new WsprBandInfo(15, WsprBand::Band_15m, 21096100ULL));
	add(new WsprBandInfo(17, WsprBand::Band_17m, 18106100ULL));
	add(new WsprBandInfo(20, WsprBand::Band_20m, 14097100ULL));
	add(new WsprBandInfo(30, WsprBand::Band_30m, 10140200ULL));
	add(new WsprBandInfo(40, WsprBand::Band_40m, 7040100ULL));
	// 60m unsupported for now, since there is disagreement over band allocation and WSPR frequency between countries
	// If you want to use 60m, check your local band plan to see which frequency you are allowed to transmit on, and adjust the value below if necessary. Permitted frequencies may include 5288700 or 5366200 depending on your location.
	//add(new WsprBandInfo(60, WsprBand::Band_60m, 5288700ULL));
	add(new WsprBandInfo(80, WsprBand::Band_80m, 3594100ULL));
	add(new WsprBandInfo(160, WsprBand::Band_160m, 1838100ULL));
	add(new WsprBandInfo(630, WsprBand::Band_630m, 475700ULL));
}

WsprBandInfoStore::~WsprBandInfoStore()
{
	for (auto it : bands) {
		delete it.second;
	}
}

void WsprBandInfoStore::add(WsprBandInfo *band)
{
	bands.insert(std::make_pair(band->bandId, band));
}
