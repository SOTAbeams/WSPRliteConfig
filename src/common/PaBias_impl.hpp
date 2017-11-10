#ifndef common_PaBias_impl_h
#define common_PaBias_impl_h

#include "common/PaBias.hpp"
#include "common/MathUtil.hpp"

#include <map>
#include <cstdint>
#include <utility>
#include <type_traits>

namespace PaBias
{

class error_NoData : public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
	error_NoData() : error_NoData("no data found") {}
};

class DataPoint
{
public:
	uint64_t freq;
	double power_dBm;
	uint16_t paBias;
	uint8_t driveStrength=3;

	DataPoint();
	DataPoint(uint64_t freq_, double power_dBm_, uint16_t paBias_, uint8_t driveStrength_=3);

	void insertData(const DataPoint& pt);
	double query(const Query& q);

	DataPoint(const DataPoint&) = default;
	DataPoint& operator=(const DataPoint&) = default;
	DataPoint(DataPoint&&) = default;
	DataPoint& operator=(DataPoint&&) = default;
};


class DataIndex
{
public:
	virtual double query(const Query& q) = 0;
	virtual void insertData(const DataPoint &pt) = 0;
	virtual ~DataIndex() {}
};

template<class Type_K, class Type_V>
class DiscreteDataIndex_impl : public DataIndex
{
protected:
	using T_Map = std::map<Type_K, Type_V*>;
	T_Map data;

	virtual Type_K getKey(const DataPoint& pt) = 0;
	virtual Type_K getKey(const Query& q) = 0;

public:
	bool hasKey(Type_K k)
	{
		return (data.count(k)>0);
	}

	void insertData(const DataPoint &pt) override
	{
		Type_K k = getKey(pt);
		if (data.count(k)==0)
			data.insert(std::make_pair(k, new Type_V()));
		data.find(k)->second->insertData(pt);
	}

	double query(const Query& q) override
	{
		typename T_Map::iterator lowEntry=data.end(), highEntry=data.end();
		Type_K search_k = getKey(q);

		auto it = data.find(search_k);
		if (it==data.end())
			throw error_NoData();
		return it->second->query(q);
	}
	double query_allMin(const Query& q)
	{
		double x;
		bool foundOne = false;
		for (auto it : data)
		{
			double test_x = it.second->query(q);
			if (!foundOne || test_x < x)
				x = test_x;
			foundOne = true;
		}
		if (!foundOne)
			throw error_NoData();
		return x;
	}
	double query_allMax(const Query& q)
	{
		double x;
		bool foundOne = false;
		for (auto it : data)
		{
			double test_x = it.second->query(q);
			if (!foundOne || test_x > x)
				x = test_x;
			foundOne = true;
		}
		if (!foundOne)
			throw error_NoData();
		return x;
	}

	virtual ~DiscreteDataIndex_impl()
	{
		for (auto it : data)
		{
			delete it.second;
		}
	}
};

template<class Type_K, class Type_V>
class InterpolatingDataIndex_impl : public DataIndex
{
protected:
	using T_Map = std::map<Type_K, Type_V*>;
	T_Map data;

	virtual Type_K getKey(const DataPoint& pt) = 0;
	virtual Type_K getKey(const Query& q) = 0;

public:
	void insertData(const DataPoint &pt) override
	{
		Type_K k = getKey(pt);
		if (data.count(k)==0)
			data.insert(std::make_pair(k, new Type_V()));
		data.find(k)->second->insertData(pt);
	}

	double query(const Query& q) override
	{
		typename T_Map::iterator lowEntry=data.end(), highEntry=data.end();
		Type_K search_k = getKey(q);

		// There is probably a better way of searching, but this is not a speed critical bit of code
		for (typename T_Map::iterator it=data.begin(); it!=data.end(); ++it)
		{
			if (it->first <= search_k && (lowEntry==data.end() || it->first > lowEntry->first))
				lowEntry = it;
			if (it->first >= search_k && (highEntry==data.end() || it->first < highEntry->first))
				highEntry = it;
		}

		if (lowEntry==data.end() && highEntry==data.end()) {
			throw error_NoData();
		}

		// If the search key is outside the bounds of the available data, use the closest available data point
		if (lowEntry==data.end()) {
			lowEntry = highEntry;
		}
		if (highEntry==data.end()) {
			highEntry = lowEntry;
		}

		return MathUtil::linInterpolate(lowEntry->first, lowEntry->second->query(q),
									   highEntry->first, highEntry->second->query(q), search_k);
	}

	virtual ~InterpolatingDataIndex_impl()
	{
		for (auto it : data)
		{
			delete it.second;
		}
	}
};

class DataIndex_Power : public InterpolatingDataIndex_impl<double, DataPoint>
{
protected:
	using Type_K = double;
	Type_K getKey(const DataPoint& pt) override
	{
		return pt.power_dBm;
	}
	Type_K getKey(const Query& q) override
	{
		return q.power_dBm;
	}

	double getMinPower_dBm();
	double getMaxPower_dBm();
public:
	double query(const Query& q) override;
};

class DataIndex_Frequency : public InterpolatingDataIndex_impl<uint64_t, DataIndex_Power>
{
protected:
	using Type_K = uint64_t;
	Type_K getKey(const DataPoint& pt) override;
	Type_K getKey(const Query& q) override;
};

class DataIndex_DriveStrength : public DiscreteDataIndex_impl<uint8_t, DataIndex_Frequency>
{
protected:
	using Type_K = uint8_t;
	using Type_V = DataIndex_Frequency;
	Type_K getKey(const DataPoint& pt) override;
	Type_K getKey(const Query& q) override;
public:
	double query(const Query& q) override;
};

class DataIndex_Band : public DiscreteDataIndex_impl<uint8_t, DataIndex_DriveStrength>
{
protected:
	using Type_K = uint8_t;
	using Type_V = DataIndex_DriveStrength;
	Type_K getKey(const DataPoint& pt) override;
	Type_K getKey(const Query& q) override;
public:
	bool hasBand(WsprBand b);
};



class Data_Classic : public DataIndex_Frequency // freq, power
{
public:
	Data_Classic();
};

class Data_Flexi_Direct : public DataIndex_DriveStrength // drive strength, freq, power
{
public:
	Data_Flexi_Direct();
};

class Data_Flexi_LpfKit : public DataIndex_Band // band, drive strength, (freq), power
{
	// There is no interpolation between frequencies at the moment, since only one frequency has been measured for each band.
	// But this should be sufficient considering the natural variation between units, and the variation in power with USB voltage, which will be significantly greater than any increased accuracy from refining bias levels within the narrow bandwidth allocated to WSPR - there is not much change in required bias over such a small frequency range.
public:
	Data_Flexi_LpfKit();
};


class Data
{
protected:
	Data_Classic data_classic;
	Data_Flexi_Direct data_flexi_direct;
	Data_Flexi_LpfKit data_flexi_lpfKit;

public:
	Data();
	double query(const Query& q);
	bool hasData(Query& q);
	virtual ~Data();
};

extern Data data;

}


#endif
