#include "RandomFreqGenerator.hpp"

#include <cmath>
#include <map>
#include <chrono>
#include <iostream>

RandomFreqGenerator randomFreqGenerator;

namespace FreqUsageData
{

class DataPoint
{
public:
	uint64_t freq;
	double usage;
};

class BandData
{
public:
	std::map<uint64_t, DataPoint> data;

	// Get a number (range 0-1) indicating how busy this frequency is, relative to the busiest frequency in this band
	double getUsageProportion(uint64_t freq);
	void insertPoint(DataPoint pt);

	double maxUsage = 0;
};

class Data
{
public:
	int binSize;
	std::map<uint64_t, BandData*> data;
	BandData* getBandData(uint64_t centreFreq);
	void insertPoint(DataPoint pt);
	Data();
	~Data();
};

Data data;




BandData* Data::getBandData(uint64_t centreFreq)
{
	auto it = data.find(centreFreq);
	if (it==data.end()) {
		return nullptr;
	}
	return it->second;
}

void Data::insertPoint(DataPoint pt)
{
	WsprBandInfo *band = WsprBandInfo::findByFreq(pt.freq);
	uint64_t centreFreq = band ? band->centreFreq : 0;
	auto it = data.find(centreFreq);
	if (it==data.end()) {
		it = data.insert(std::make_pair(centreFreq, new BandData())).first;
	}
	it->second->insertPoint(pt);
}

Data::Data()
{
	binSize = 5;
	std::vector<DataPoint> points = {
		#include "freqData/inc/all.inc"
	};
	for (DataPoint pt : points)
	{
		insertPoint(pt);
	}
}

Data::~Data()
{
	for (auto it : data)
	{
		delete it.second;
	}
}

double BandData::getUsageProportion(uint64_t freq)
{
	auto it = data.find(freq);
	if (it==data.end()) {
		return 0;
	}
	return it->second.usage / maxUsage;
}

void BandData::insertPoint(DataPoint pt)
{
	data.insert(std::make_pair(pt.freq, pt));
	if (pt.usage > maxUsage)
		maxUsage = pt.usage;
}

}


int RandomFreqGenerator::getBandwidth(std::shared_ptr<DeviceModel> deviceModel, uint64_t centreFreq)
{
	// Default value, for devices that have not had their oscillator calibrated
	int bandwidth = 100;

	if (deviceModel->hasCalibratedOscillator())
	{
		WsprBandInfo *band = WsprBandInfo::findByCentre(centreFreq);
		if (band==nullptr)
			return bandwidth;

		// Output frequency is obtained by scaling the oscillator frequency, so oscillator frequency errors scale to smaller output frequency errors at lower output frequencies.
		// This means the lower the output frequency, the larger the safe bandwidth for frequency selection, since the generated frequency can be produced with better accuracy. It can be closer to the edge of the WSPR allocation without risking falling outside the permitted range.

		if (band->centreFreq<30e6)
		{
			// 1ppm margin each side, plus a little extra
			bandwidth = 200 - 30 - 2e-6 * band->centreFreq;
		}
		else
		{
			// Reasonably narrow bandwidth for 50 MHz band
			bandwidth = 100;
		}
	}

	return bandwidth;
}

uint64_t RandomFreqGenerator::uniformGen(std::shared_ptr<DeviceModel> deviceModel, uint64_t centreFreq)
{
	WsprBandInfo *band = WsprBandInfo::findByCentre(centreFreq);
	if (band==nullptr)
		return 0;

	int bandwidth = getBandwidth(deviceModel, centreFreq);
	std::uniform_int_distribution<> randDist(-bandwidth/2, bandwidth/2);
	return band->centreFreq + randDist(rng);
}

uint64_t RandomFreqGenerator::usageBasedGen(std::shared_ptr<DeviceModel> deviceModel, uint64_t centreFreq)
{
	// Rejection sampling, based on data about how often the different frequencies available within a band are used.

	// Using rejection sampling instead of inverse transform sampling since it is easier to implement and this is not a speed critical function

	// How much importance to give the usage data. 1=generate entirely according to the usage data and never generate a frequency in the region of max usage; 0=usage data is ignored
	double usageWeight = 0.95;

	FreqUsageData::BandData *bandUsageData = FreqUsageData::data.getBandData(centreFreq);
	int binSize = FreqUsageData::data.binSize;
	if (bandUsageData==nullptr)
		return 0;
	WsprBandInfo *band = WsprBandInfo::findByCentre(centreFreq);
	if (band==nullptr)
		return 0;
	int bandwidth = getBandwidth(deviceModel, centreFreq);

	std::uniform_int_distribution<> freqOffsetDist(-bandwidth/2, bandwidth/2);
	std::uniform_real_distribution<> acceptDist(0.0, 1.0); // 0.0 <= x < 1.0

	for (int attempt=0; attempt<1000; attempt++) {
		uint64_t freq = band->centreFreq + freqOffsetDist(rng);
		uint64_t binnedFreq = (uint64_t)std::round((double)freq / binSize) * binSize;
		double acceptProb = 1.0 - usageWeight*bandUsageData->getUsageProportion(binnedFreq);
		//std::cout << "FreqGen f=" << freq << " P(accept)=" << acceptProb << std::endl;
		if (acceptProb>acceptDist(rng))
			return freq;
	}
	return 0;
}

RandomFreqGenerator::RandomFreqGenerator() :
	rng(std::chrono::high_resolution_clock::now().time_since_epoch().count())
{

}

uint64_t RandomFreqGenerator::generate(std::shared_ptr<DeviceModel> deviceModel, uint64_t centreFreq)
{
	if (deviceModel->hasCalibratedOscillator())
	{
		uint64_t freq = usageBasedGen(deviceModel, centreFreq);
		if (freq!=0)
			return freq;
	}
	return uniformGen(deviceModel, centreFreq);
}
