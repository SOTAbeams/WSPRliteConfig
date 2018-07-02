#ifndef common_StrUtil_h
#define common_StrUtil_h

#include <string>
#include <cstdint>
#include <vector>

namespace StrUtil
{

std::string doubleToString(double x);
int64_t stringToInt(std::string txt);
uint64_t stringToUint(std::string txt);
double stringToDouble(std::string txt);

std::string power_dBmToRoundedString(double dBm);

void splitString(std::vector<std::string> &result, const std::string &txt, char delim);

std::string toUpper(std::string s);
std::string trim(std::string s);
std::vector<uint8_t> hex_decode(std::string s);
bool hex_validate(std::string s);
std::string url_encode(std::string s);

bool startsWith(std::string txt, std::string prefix);

}

#endif
