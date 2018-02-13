#ifndef common_WsprBand_h
#define common_WsprBand_h

#include <cstdint>
#include <map>
#include <vector>
#include <string>

enum class WsprBand
{
	Band_6m,
	Band_10m,
	Band_12m,
	Band_15m,
	Band_17m,
	Band_20m,
	Band_30m,
	Band_40m,
	Band_60m_52887,
	Band_60m_53662,
	Band_80m,
	Band_160m,
	Band_630m,
};

class WsprBandInfo
{
protected:
	WsprBand bandId;
public:
	uint32_t approxWl;
	uint64_t centreFreq;
	WsprBandInfo(uint32_t approxWl_, WsprBand bandId_, uint64_t centreFreq_);
	bool containsFreq(uint32_t f) const;
	uint64_t getMinFreq() const;
	uint64_t getMaxFreq() const;
	std::string toString_shortMHz();
	WsprBand getBandId() const;
	// Get an integer code for the band, based on WSPRnet definition: "an integer representing the MHz component of the frequency with a special case for LF (-1: LF, 0: MF, 1: 160m, 3: 80m, 5: 60m, 7: 40m, 10: 30m, ...)."
	uint32_t getBandCode() const;

	static WsprBandInfo* findByCentre(uint64_t centreFreq);
	static WsprBandInfo* findByFreq(uint64_t f);
	static WsprBandInfo* findById(WsprBand id);
};

class WsprBandInfoStore
{
public:
	std::map<WsprBand, WsprBandInfo*> bandsById;
	std::map<uint64_t, WsprBandInfo*> bandsByCentre;
	std::vector<WsprBandInfo*> allBands;
	WsprBandInfoStore();
	~WsprBandInfoStore();
	void add(WsprBandInfo *band);
};

#endif
