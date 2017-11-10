#ifndef common_PaBias_h
#define common_PaBias_h

#include "common/Version.hpp"
#include "common/Device.hpp"

#include <map>
#include <cstdint>
#include <utility>
#include <memory>

namespace PaBias
{

enum class QueryType
{
	PaBias,
	MinPower_dBm,
	MaxPower_dBm,
};

class Query
{
public:
	QueryType type;
	PaBiasSource biasSource;
	DeviceVersion deviceVersion;
	FirmwareVersion firmwareVersion;
	double freq;
	double power_dBm;
};

uint16_t get(DeviceInfo &deviceInfo, PaBiasSource biasSource, uint64_t freq, double power_dBm);
double getMinPower_dBm(DeviceInfo &deviceInfo, PaBiasSource biasSource, uint64_t freq);
double getMaxPower_dBm(DeviceInfo &deviceInfo, PaBiasSource biasSource, uint64_t freq);

bool hasData(DeviceInfo &deviceInfo, PaBiasSource biasSource, uint64_t freq);

}


#endif
