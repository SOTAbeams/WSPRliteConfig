#include "WsprCallsign.hpp"
#include "StrUtil.hpp"

std::string WsprCallsign::canonicalFormat(std::string s)
{
	return StrUtil::toUpper(StrUtil::trim(s));
}
