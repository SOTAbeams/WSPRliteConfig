#ifndef common_PaBias_h
#define common_PaBias_h

#include <set>
#include <vector>
#include <cstdint>

namespace PaBias
{

class DataPoint
{
public:
	uint64_t freq;
	int8_t power_dBm;
	uint16_t paBias;
	DataPoint();
	DataPoint(uint64_t freq_, int8_t power_dBm_, uint16_t paBias_);
};

class Data
{
public:
	std::vector<DataPoint> points;
	std::set<int8_t> powerVals;
	Data();
};

extern Data data;

uint16_t get(uint64_t freq, int8_t power_dBm);

}


#endif
