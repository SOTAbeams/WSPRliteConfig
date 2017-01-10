#include "Task_FactoryReset.hpp"

void Task_FactoryReset::task()
{
	send(DeviceComm::genMsg_write_str(DeviceComm::VarId::WSPR_callsign, "")).assert_ack();
	send(DeviceComm::genMsg_write_str(DeviceComm::VarId::WSPR_locator, "")).assert_ack();
	send(DeviceComm::genMsg_write_int<int8_t>(DeviceComm::VarId::WSPR_outputPower, 23)).assert_ack();
	send(DeviceComm::genMsg_write_int<uint16_t>(DeviceComm::VarId::WSPR_paBias, 411)).assert_ack();
	send(DeviceComm::genMsg_write_int<int8_t>(DeviceComm::VarId::WSPR_reportPower, 23)).assert_ack();
	send(DeviceComm::genMsg_write_int<uint64_t>(DeviceComm::VarId::WSPR_maxTxDuration, 3600ULL*24*3)).assert_ack();
	send(DeviceComm::genMsg_write_int<uint8_t>(DeviceComm::VarId::WSPR_txPct, 20)).assert_ack();
	send(DeviceComm::genMsg_write_int<uint64_t>(DeviceComm::VarId::WSPR_txFreq, 14097100ULL)).assert_ack();
}

Task_FactoryReset::~Task_FactoryReset()
{}
