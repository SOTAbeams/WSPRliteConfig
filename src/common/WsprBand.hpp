#ifndef common_WsprBand_h
#define common_WsprBand_h

#include <cstdint>
#include <map>
#include <string>

enum class WsprBand
{
	Band_6m=50,
	Band_10m=28,
	Band_12m=24,
	Band_15m=21,
	Band_17m=18,
	Band_20m=14,
	Band_30m=10,
	Band_40m=7,
	Band_60m=5,
	Band_80m=3,
	Band_160m=1,
	Band_630m=0,
};

class WsprBandInfo
{
public:
	uint32_t approxWl;
	WsprBand bandId;
	uint64_t centreFreq;
	WsprBandInfo(uint32_t approxWl_, WsprBand bandId_, uint64_t centreFreq_);
	bool containsFreq(uint32_t f) const;
	uint64_t getMinFreq() const;
	uint64_t getMaxFreq() const;
	std::string toString_shortMHz();

	static WsprBandInfo* find(WsprBand bandId);
};

class WsprBandInfoStore
{
public:
	std::map<WsprBand, WsprBandInfo*> bands;
	WsprBandInfoStore();
	~WsprBandInfoStore();
	void add(WsprBandInfo *band);
};

#endif
