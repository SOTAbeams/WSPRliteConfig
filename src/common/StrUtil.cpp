#include "StrUtil.hpp"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <cmath>

int64_t StrUtil::stringToInt(std::string txt)
{
	return std::stoll(txt);
}

uint64_t StrUtil::stringToUint(std::string txt)
{
	return std::stoull(txt);
}

std::string StrUtil::toUpper(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
	return s;
}

std::vector<uint8_t> StrUtil::hex_decode(std::string s)
{
	size_t len = s.length();
	std::vector<uint8_t> data;
	for(int i=0; i< len; i+=2)
	{
		std::string byte = s.substr(i,2);
		if (std::isxdigit(byte[0]) && std::isxdigit(byte[1]))
		{
			uint8_t chr = strtol(byte.c_str(), nullptr, 16);
			data.push_back(chr);
		}
		else
		{
			break;
		}
	}
	return data;
}

bool StrUtil::hex_validate(std::string s)
{
	for (char c: s)
	{
		if (!std::isxdigit(c))
			return false;
	}
	return true;
}


double StrUtil::stringToDouble(std::string txt)
{
	return std::stod(txt);
}

std::string StrUtil::doubleToString(double x)
{
	std::stringstream ss;
	ss << x;
	return ss.str();
}

std::string StrUtil::power_dBmToRoundedString(double dBm)
{
	double mwatts = std::pow(10, dBm/10);
	if (dBm<30)
	{
		int roundScale = std::pow(10, std::floor(dBm/10));
		return doubleToString(round(mwatts/roundScale)*roundScale) + " mW";
	}
	else
	{
		int roundScale = std::pow(10, std::floor(dBm/10)-3);
		return doubleToString(round(mwatts/1000/roundScale)*roundScale) + " W";
	}
}

void StrUtil::splitString(std::vector<std::string> &result, const std::string &txt, char delim)
{
	std::istringstream ss(txt);
	std::string item;
	result.clear();
	while (std::getline(ss, item, delim)) {
		result.push_back(item);
	}
}

bool StrUtil::startsWith(std::string txt, std::string prefix)
{
	return (txt.compare(0, prefix.size(), prefix)==0);
}

