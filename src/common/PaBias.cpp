#include "PaBias.hpp"

#include <cmath>

PaBias::DataPoint::DataPoint()
{}

PaBias::DataPoint::DataPoint(uint64_t freq_, int8_t power_dBm_, uint16_t paBias_) :
	freq(freq_), power_dBm(power_dBm_), paBias(paBias_)

{}

PaBias::Data PaBias::data;

PaBias::Data::Data()
{
	points = {
		// Measurements of the correct bias value for a given power and frequency
		// (PaBiasData.inc was generated from PaBiasData.csv)
		#include "PaBiasData.inc"
	};

	// Create a list of valid power levels
	for (DataPoint pt : points)
	{
		powerVals.insert(pt.power_dBm);
	}
}

uint16_t PaBias::get(uint64_t freq, int8_t power_dBm)
{
	if (data.powerVals.find(power_dBm)==data.powerVals.end())
	{
		// Power value does not exist in the available data, find the closest one
		// Could do some interpolation instead, but it's a bit pointless since the WSPR protocol only supports certain reported power levels and measurements have been done for all of those that the WSPRlite can output.
		int bestDelta = 1000;
		int bestPower = 23;
		for (int p : data.powerVals)
		{
			int delta = std::abs(p - power_dBm);
			if (delta<bestDelta)
			{
				bestDelta = delta;
				bestPower = p;
			}
		}
		power_dBm = bestPower;
	}

	// Find the data points matching the requested power level that are closest in frequency to the desired freq
	DataPoint nearestLF, nearestHF;
	bool foundLF=false, foundHF=false;
	for (DataPoint pt : data.points)
	{
		if (pt.power_dBm != power_dBm)
			continue;
		if (pt.freq<=freq && (!foundLF || pt.freq > nearestLF.freq))
		{
			foundLF = true;
			nearestLF = pt;
		}
		if (pt.freq>=freq && (!foundHF || pt.freq < nearestHF.freq))
		{
			foundHF = true;
			nearestHF = pt;
		}
	}

	// If exact match found, return that
	if (foundLF && nearestLF.freq==freq)
		return nearestLF.paBias;

	// If only one neighbouring point found (so outside supported range of frequencies), return that
	if (foundLF && !foundHF)
		return nearestLF.paBias;
	if (!foundLF && foundHF)
		return nearestHF.paBias;

	// Interpolate
	// Note that there are some regions where linear interpolation will be inaccurate if there are not enough data points, e.g. below 1MHz
	double x = (double)(freq-nearestLF.freq)/(nearestHF.freq-nearestLF.freq);
	return std::round(x*nearestHF.paBias + (1.0-x)*nearestLF.paBias);
}

