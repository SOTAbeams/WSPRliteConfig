#include "WsprCallsign.hpp"
#include "StrUtil.hpp"

#include <regex>

namespace WsprCallsign
{

static const std::string patt_basicCallsign = "[A-Z0-9 ]?[A-Z0-9][0-9][A-Z ]{0,3}";

bool isBasic(std::string x)
{
	std::regex validCallsignRegex("^"+patt_basicCallsign+"$");
	return std::regex_match(x, validCallsignRegex);
}

bool isCompound(std::string x)
{

	std::string prefixes = "[A-Z0-9 ]{0,3}";
	std::string suffixes = "[A-Z0-9]|[1-9][0-9]";
	std::regex validCallsignRegex("^(("+prefixes+")/"+patt_basicCallsign+"|"+patt_basicCallsign+"/("+suffixes+"))$");
	return std::regex_match(x, validCallsignRegex);
}

bool isValid(std::string x)
{
	return (isBasic(x) || isCompound(x));
}

std::string canonicalFormat(std::string s)
{
	return StrUtil::toUpper(StrUtil::trim(s));
}

}
