#ifndef common_device_DeviceMode_h
#define common_device_DeviceMode_h

enum class DeviceMode
{
	Init=0,
	WSPR_Pending=1,
	WSPR_Active,
	WSPR_Invalid,
	Test_ConstantTx,
	FactoryInvalid,
	HardwareFail,
	FirmwareError,
	WSPR_MorseIdent,
	Test,
};


enum class DeviceModeSub_WSPR
{
	idle=0,
	tx,
};

union DeviceModeSub
{
	DeviceModeSub_WSPR wspr;
};

#endif
