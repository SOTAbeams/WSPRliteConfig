#ifndef common_device_MsgType_h
#define common_device_MsgType_h

namespace DeviceComm
{

enum class MsgType
{
	Version=0,
	NACK,
	ACK,
	Read,
	ResponseData,
	Write,
	Reset,
	Bootloader_State,
	Bootloader_Enter,
	Bootloader_EraseAll,
	Bootloader_ErasePage,
	Bootloader_ProgramHexRec,
	Bootloader_ProgramRow,
	Bootloader_ProgramWord,
	Bootloader_CRC,
	Bootloader_ProgramResetAddr,
	DeviceMode_Get,
	DeviceMode_Set,
	DumpEEPROM,
	WSPR_GetTime,
};

// EEPROM variable IDs, for use with MsgType::Read and Write
enum class VarId
{
	MemVersion=0,
	xoFreq,
	xoFreqFactory,
	ChangeCounter,
	DeviceId,
	DeviceSecret,
	WSPR_txFreq,
	WSPR_locator,
	WSPR_callsign,
	WSPR_paBias,
	WSPR_outputPower,
	WSPR_reportPower,
	WSPR_txPct,
	WSPR_maxTxDuration,
	END,
};


}

#endif
