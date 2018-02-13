#include "PaBias_impl.hpp"
#include "common/MathUtil.hpp"
#include "common/WsprBand.hpp"

#include <cmath>
#include <vector>

using namespace PaBias;

PaBias::Data PaBias::data;


PaBias::DataPoint::DataPoint()
{}

PaBias::DataPoint::DataPoint(uint64_t freq_, double power_dBm_, uint16_t paBias_, uint8_t driveStrength_) :
	freq(freq_), power_dBm(power_dBm_), paBias(paBias_), driveStrength(driveStrength_)

{}

void DataPoint::insertData(const DataPoint &pt)
{
	*this = pt;
}

double DataPoint::query(const Query &q)
{
	switch (q.type)
	{
	case QueryType::PaBias:
		return paBias;
	case QueryType::MinPower_dBm:
	case QueryType::MaxPower_dBm:
		return power_dBm;
	}
}


double DataIndex_Power::getMinPower_dBm()
{
	for (auto it = data.begin(); it != data.end(); ++it) {
		return it->first;
	}
	throw error_NoData();
}

double DataIndex_Power::getMaxPower_dBm()
{
	for (auto it = data.rbegin(); it != data.rend(); ++it) {
		return it->first;
	}
	throw error_NoData();
}

double DataIndex_Power::query(const Query &q)
{
	if (q.type==QueryType::MinPower_dBm)
		return getMinPower_dBm();
	if (q.type==QueryType::MaxPower_dBm)
		return getMaxPower_dBm();
	return InterpolatingDataIndex_impl<double, DataPoint>::query(q);
}

DataIndex_DriveStrength::Type_K DataIndex_DriveStrength::getKey(const DataPoint &pt)
{
	return pt.driveStrength;
}

DataIndex_DriveStrength::Type_K DataIndex_DriveStrength::getKey(const Query &q)
{
	// Old firmware always uses drive strength = 3
	if (!q.firmwareVersion.supports_driveStrength())
		return 3;

	if (q.type==QueryType::PaBias)
	{
		// TODO: automatically decide this based on requested power level
		return 3;
	}

	// Other query types should be handled in query(), but return a sensible default here
	return 3;
}

double DataIndex_DriveStrength::query(const Query &q)
{
	if (q.firmwareVersion.supports_driveStrength())
	{
		if (q.type==QueryType::MinPower_dBm)
			return query_allMin(q);
		if (q.type==QueryType::MaxPower_dBm)
			return query_allMax(q);
	}
	return DiscreteDataIndex_impl<Type_K, Type_V>::query(q);
}

PaBias::DataIndex_Frequency::Type_K PaBias::DataIndex_Frequency::getKey(const PaBias::DataPoint &pt)
{
	return pt.freq;
}

DataIndex_Frequency::Type_K DataIndex_Frequency::getKey(const Query &q)
{
	return q.freq;
}


// There have been some slight changes in band frequency, but since the new frequency is close to the old one, the old data should suffice for now.
// The data is therefore indexed by the WSPRnet band code, which for 630m and higher frequencies is an integer representing the MHz component of the frequency. This code stays the same for each band even when the frequency changes slightly.

DataIndex_Band::Type_K DataIndex_Band::getKey(const DataPoint &pt)
{
	return (int)std::floor(pt.freq/1e6);
}

DataIndex_Band::Type_K DataIndex_Band::getKey(const Query &q)
{
	return (int)std::floor(q.freq/1e6);
}

bool DataIndex_Band::hasBand(WsprBand b)
{
	WsprBandInfo *bandInfo = WsprBandInfo::findById(b);
	if (!bandInfo)
		return false;
	return hasKey(bandInfo->getBandCode());
}



// Measurements of power output versus bias value and frequency:

Data_Classic::Data_Classic()
{
	std::vector<DataPoint> points = {
		#include "biasData/inc/classic.inc"
	};

	for (DataPoint pt : points)
	{
		insertData(pt);
	}
}

Data_Flexi_Direct::Data_Flexi_Direct()
{
	std::vector<DataPoint> points = {
		#include "biasData/inc/flexi-direct.inc"
	};

	for (DataPoint pt : points)
	{
		insertData(pt);
	}
}

Data_Flexi_LpfKit::Data_Flexi_LpfKit()
{
	std::vector<DataPoint> points = {
		#include "biasData/inc/flexi-lpfkit.inc"
	};

	for (DataPoint pt : points)
	{
		insertData(pt);
	}
}


Data::Data()
{

}

double Data::query(const Query &q)
{
	if (q.deviceVersion.has_20mFilter())
		return data_classic.query(q);
	else
	{
		if (q.biasSource==PaBiasSource::Default || q.biasSource==PaBiasSource::SbLpfKit)
		{
			try {
				return data_flexi_lpfKit.query(q);
			} catch (error_NoData &e) {
				return data_flexi_direct.query(q);
			}
		}
		else
		{
			return data_flexi_direct.query(q);
		}
	}
}

bool Data::hasData(Query &q)
{
	// TODO: add another query type for this?
	q.type = QueryType::MaxPower_dBm;
	if (q.deviceVersion.has_20mFilter())
		return true;
	else
	{
		if (q.biasSource==PaBiasSource::Default || q.biasSource==PaBiasSource::SbLpfKit)
		{
			try {
				data_flexi_lpfKit.query(q);
				return true;
			} catch (error_NoData &e) {
				return false;
			}
		}
		else
		{
			return true;
		}
	}
}

Data::~Data() {}
