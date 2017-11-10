#include "PaBias.hpp"
#include "PaBias_impl.hpp"
#include "common/MathUtil.hpp"

#include <cmath>
#include <vector>
#include <iostream>

uint16_t PaBias::get(DeviceInfo &deviceInfo, PaBiasSource biasSource, uint64_t freq, double power_dBm)
{
	PaBias::Query q;
	q.deviceVersion = deviceInfo.deviceVersion;
	q.firmwareVersion = deviceInfo.firmwareVersion;
	q.freq = freq;
	q.biasSource = biasSource;
	q.power_dBm = power_dBm;


	q.type = PaBias::QueryType::PaBias;
	double result = PaBias::data.query(q);
	/*q.type = PaBias::QueryType::MaxPower_dBm;
	double maxPower = PaBias::data.query(q);
	q.type = PaBias::QueryType::MinPower_dBm;
	double minPower = PaBias::data.query(q);
	std::cout << "PA bias f=" << freq << " P=" << power_dBm << "dBm (available range " << minPower << " to " << maxPower << ") bias=" << result << std::endl;*/
	return std::round(result);
}



double PaBias::getMinPower_dBm(DeviceInfo &deviceInfo, PaBiasSource biasSource, uint64_t freq)
{
	PaBias::Query q;
	q.deviceVersion = deviceInfo.deviceVersion;
	q.firmwareVersion = deviceInfo.firmwareVersion;
	q.freq = freq;
	q.biasSource = biasSource;

	q.type = PaBias::QueryType::MinPower_dBm;
	double result = PaBias::data.query(q);
	return std::round(result);
}

double PaBias::getMaxPower_dBm(DeviceInfo &deviceInfo, PaBiasSource biasSource, uint64_t freq)
{
	PaBias::Query q;
	q.deviceVersion = deviceInfo.deviceVersion;
	q.firmwareVersion = deviceInfo.firmwareVersion;
	q.freq = freq;
	q.biasSource = biasSource;

	q.type = PaBias::QueryType::MaxPower_dBm;
	double result = PaBias::data.query(q);
	return std::round(result);
}

bool PaBias::hasData(DeviceInfo &deviceInfo, PaBiasSource biasSource, uint64_t freq)
{
	PaBias::Query q;
	q.deviceVersion = deviceInfo.deviceVersion;
	q.firmwareVersion = deviceInfo.firmwareVersion;
	q.freq = freq;
	q.biasSource = biasSource;

	return PaBias::data.hasData(q);
}
