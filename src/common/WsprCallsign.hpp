#ifndef common_WsprCallsign_h
#define common_WsprCallsign_h

#include <string>

namespace WsprCallsign
{

bool isBasic(std::string x);
bool isCompound(std::string x);
bool isValid(std::string x);
std::string canonicalFormat(std::string s);

}

#endif
