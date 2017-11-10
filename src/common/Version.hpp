#ifndef common_DeviceVersion_h
#define common_DeviceVersion_h

#include "RelOps.hpp"
#include "common/WsprBand.hpp"
#include "common/device/MsgType.hpp"

#include <cstdint>
#include <string>

namespace DeviceComm
{
class Data;
}


class DeviceVersion
{
public:
	uint32_t productId;
	uint32_t productRevision;
	uint32_t bootloaderVersion;

	void loadFromMsgData(DeviceComm::Data& data, size_t i=0);

	bool has_20mFilter() const;
};


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

	bool supports_band(WsprBand b) const;
	bool supports_msg(DeviceComm::MsgType t) const;
	bool supports_cwId() const;
	bool supports_driveStrength() const;
	bool supports_varId(DeviceComm::VarId v) const;
	bool supports_device(DeviceVersion deviceVersion) const;
};

bool operator==(const FirmwareVersion &a, const FirmwareVersion &b);
bool operator<(const FirmwareVersion &a, const FirmwareVersion &b);


#endif
