#include "Version.hpp"
#include "Device.hpp"
#include "StrUtil.hpp"

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

bool FirmwareVersion::supports_band(WsprBand b) const
{
	if (b == WsprBand::Band_630m) {
		return ((*this) >= FirmwareVersion(1, 0, 6, 20170130));
	}
	if (WsprBandInfo::findById(b)->centreFreq > 15e6) {
		return ((*this) >= FirmwareVersion(1, 1, 0, 20170605));
	}
	return true;
}

bool FirmwareVersion::supports_msg(DeviceComm::MsgType t) const
{
	using namespace DeviceComm;
	if (t==MsgType::DeviceMode_Get || t==MsgType::DeviceMode_Set)
		return ((*this) >= FirmwareVersion(1, 0, 4, 20170109));
	if (t==MsgType::WSPR_GetTime)
		return ((*this) >= FirmwareVersion(1, 1, 1,20170803));
	if (t==MsgType::TestCmd)
		return ((*this) >= FirmwareVersion(1, 1, 2, 20171009));
	if ((int)t <= (int)MsgType::DeviceMode_Set)
		return true;
	return false;
}

bool FirmwareVersion::supports_cwId() const
{
	return ((*this) >= FirmwareVersion(1,0,6,20170130));
}

bool FirmwareVersion::supports_device(DeviceVersion deviceVersion) const
{
	if (deviceVersion.productId==1 && deviceVersion.productRevision==1)
	{
		// Supported by all released firmware
		return true;
	}
	if ((deviceVersion.productId==1 && deviceVersion.productRevision==2) ||
		(deviceVersion.productId==2 && deviceVersion.productRevision==1))
	{
		// Uses a different EEPROM which is not supported by older firmware
		return ((*this) >= FirmwareVersion(1,1,0,20170605));
	}
	return false;
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

bool DeviceVersion::has_20mFilter() const
{
	return (productId == 1);
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

bool FirmwareVersion::supports_driveStrength() const
{
	// PA bias code supports this, but firmware does not yet
	return false;
}

bool FirmwareVersion::supports_varId(DeviceComm::VarId v) const
{
	if ((int)v <= (int)DeviceComm::VarId::WSPR_maxTxDuration)
		return true;
	if (v==DeviceComm::VarId::CwId_Freq || v==DeviceComm::VarId::CwId_Callsign)
		return ((*this) >= FirmwareVersion(1,0,6,20170130));
	if (v==DeviceComm::VarId::PaBiasSource)
		return ((*this) >= FirmwareVersion(1,1,3,20171012));
	return false;
}
