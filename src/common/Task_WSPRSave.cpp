#include "Task_WSPRSave.hpp"

#include "common/StrUtil.hpp"

void Task_WSPRSave::task()
{
	send(DeviceComm::genMsg_write_str(DeviceComm::VarId::WSPR_callsign, newCfg.callsign)).assert_ack();
	send(DeviceComm::genMsg_write_str(DeviceComm::VarId::WSPR_locator, newCfg.locator)).assert_ack();
	send(DeviceComm::genMsg_write_int<int8_t>(DeviceComm::VarId::WSPR_outputPower, newCfg.outputPower_dBm)).assert_ack();
	send(DeviceComm::genMsg_write_int<uint16_t>(DeviceComm::VarId::WSPR_paBias, newCfg.paBias)).assert_ack();
	send(DeviceComm::genMsg_write_int<int8_t>(DeviceComm::VarId::WSPR_reportPower, newCfg.reportedPower_dBm)).assert_ack();
	send(DeviceComm::genMsg_write_int<uint64_t>(DeviceComm::VarId::WSPR_maxTxDuration, newCfg.maxRuntime)).assert_ack();
	send(DeviceComm::genMsg_write_int<uint8_t>(DeviceComm::VarId::WSPR_txPct, newCfg.transmitPercent)).assert_ack();
	send(DeviceComm::genMsg_write_int<uint64_t>(DeviceComm::VarId::WSPR_txFreq, newCfg.transmitFreq)).assert_ack();
	if (StrUtil::toUpper(newCfg.callsign) != StrUtil::toUpper(deviceModel->config.callsign))
		newCfg.changeCounter = deviceModel->config.changeCounter+1;
	else
		newCfg.changeCounter = deviceModel->config.changeCounter;
	send(DeviceComm::genMsg_write_int<uint64_t>(DeviceComm::VarId::ChangeCounter, newCfg.changeCounter)).assert_ack();

	deviceModel->config = newCfg;
}

Task_WSPRSave::~Task_WSPRSave()
{}
