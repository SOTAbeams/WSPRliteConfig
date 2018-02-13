#ifndef common_RandomFreqGenerator_h
#define common_RandomFreqGenerator_h

#include "common/WsprBand.hpp"
#include "common/Device.hpp"

#include <memory>
#include <cstdint>
#include <random>

class RandomFreqGenerator
{
protected:
	std::mt19937 rng;
	int getBandwidth(std::shared_ptr<DeviceModel> deviceModel, uint64_t centreFreq);
	uint64_t uniformGen(std::shared_ptr<DeviceModel> deviceModel, uint64_t centreFreq);
	uint64_t usageBasedGen(std::shared_ptr<DeviceModel> deviceModel, uint64_t centreFreq);
public:
	RandomFreqGenerator();
	uint64_t generate(std::shared_ptr<DeviceModel> deviceModel, uint64_t centreFreq);
};

extern RandomFreqGenerator randomFreqGenerator;

#endif
