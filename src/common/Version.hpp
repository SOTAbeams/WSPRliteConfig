#ifndef common_DeviceVersion_h
#define common_DeviceVersion_h

#include "RelOps.hpp"

#include <cstdint>
#include <string>

namespace DeviceComm
{
class Data;
}

class FirmwareVersion : public RelOps<FirmwareVersion>
{
public:
	uint32_t majorVersion, minorVersion, patchVersion;
	uint32_t releaseDate;

	FirmwareVersion();
	FirmwareVersion(uint32_t major_, uint32_t minor_, uint32_t patch_, uint32_t date_);
	std::string toString();
	void loadFromMsgData(DeviceComm::Data& data, size_t i=0);
	bool isValid() const;

	bool supports_deviceMode() const;
	bool supports_cwId() const;
};

bool operator==(const FirmwareVersion &a, const FirmwareVersion &b);
bool operator<(const FirmwareVersion &a, const FirmwareVersion &b);

class DeviceVersion
{
public:
	uint32_t productId;
	uint32_t productRevision;
	uint32_t bootloaderVersion;

	void loadFromMsgData(DeviceComm::Data& data, size_t i=0);
};


#endif
