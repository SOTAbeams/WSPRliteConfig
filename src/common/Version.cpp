#include "Version.hpp"
#include "Device.hpp"
#include "StrUtil.hpp"

#include <utility>
using namespace std::rel_ops;

FirmwareVersion::FirmwareVersion() :
	majorVersion(0), minorVersion(0), patchVersion(0), releaseDate(0)
{}

FirmwareVersion::FirmwareVersion(uint32_t major_, uint32_t minor_, uint32_t patch_, uint32_t date_) :
	majorVersion(major_), minorVersion(minor_), patchVersion(patch_), releaseDate(date_)
{}

std::string FirmwareVersion::toString()
{
	return "v" + std::to_string(majorVersion) + "." + std::to_string(minorVersion) + "." + std::to_string(patchVersion) + "-" + std::to_string(releaseDate);
}

void FirmwareVersion::loadFromMsgData(DeviceComm::Data &data, size_t i)
{
	majorVersion = data.parse_int_le<uint32_t>(i);
	i += sizeof(uint32_t);
	minorVersion = data.parse_int_le<uint32_t>(i);
	i += sizeof(uint32_t);
	patchVersion = data.parse_int_le<uint32_t>(i);
	i += sizeof(uint32_t);
	releaseDate = data.parse_int_le<uint32_t>(i);
	i += sizeof(uint32_t);
}

bool FirmwareVersion::isValid() const
{
	return (majorVersion!=0xFFFFFFFF);
}

bool FirmwareVersion::supports_deviceMode() const
{
	return ((*this) >= FirmwareVersion(1,0,4,20170109));
}

bool FirmwareVersion::supports_cwId() const
{
	return ((*this) >= FirmwareVersion(1,0,6,20170130));
}

bool FirmwareVersion::supports_630m() const
{
	return ((*this) >= FirmwareVersion(1,0,6,20170130));
}

void DeviceVersion::loadFromMsgData(DeviceComm::Data &data, size_t i)
{
	productId = data.parse_int_le<uint32_t>(i);
	i += sizeof(uint32_t);
	productRevision = data.parse_int_le<uint32_t>(i);
	i += sizeof(uint32_t);
	bootloaderVersion = data.parse_int_le<uint32_t>(i);
	i += sizeof(uint32_t);
}


bool operator==(const FirmwareVersion &a, const FirmwareVersion &b)
{
	return (a.majorVersion == b.majorVersion &&
			a.minorVersion == b.minorVersion &&
			a.patchVersion == b.patchVersion &&
			a.releaseDate == b.releaseDate);
}


bool operator<(const FirmwareVersion &a, const FirmwareVersion &b)
{
	if (a.majorVersion == b.majorVersion)
	{
		if (a.minorVersion == b.minorVersion)
		{
			if (a.patchVersion == b.patchVersion)
			{
				if (a.releaseDate == b.releaseDate)
				{
					return false;
				}
				return (a.releaseDate < b.releaseDate);
			}
			return (a.patchVersion < b.patchVersion);
		}
		return (a.minorVersion < b.minorVersion);
	}
	return (a.majorVersion < b.majorVersion);
}
