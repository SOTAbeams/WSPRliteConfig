#ifndef common_dxplorer_h
#define common_dxplorer_h

#include <cstdint>
#include <string>
#include <vector>

namespace DXplorer
{

std::string generateKey(uint64_t deviceId, std::vector<uint8_t> deviceSecret, uint64_t changeCounter, std::string callsign);

}

#endif
