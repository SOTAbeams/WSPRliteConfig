#include "Task_WSPRLoad.hpp"

void Task_WSPRLoad::task()
{
	DeviceComm::MsgResponse r;

	DeviceConfig cfg = deviceModel->config;
	DeviceInfo deviceInfo = deviceModel->info;

	
	r = send(DeviceComm::MsgType::Version).assert_data();
	deviceInfo.loadVersionsFromMsg(r.msg.data);

	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::xoFreq)).assert_data();
	cfg.xoFreq = r.msg.data.parse_int_le<uint64_t>();
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::DeviceId)).assert_data();
	deviceInfo.auth.id = r.msg.data.parse_int_le<uint64_t>();
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::DeviceSecret)).assert_data();
	deviceInfo.auth.secret = r.msg.data.d;
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::ChangeCounter)).assert_data();
	cfg.changeCounter = r.msg.data.parse_int_le<uint64_t>();
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::WSPR_callsign)).assert_data();
	cfg.callsign = r.msg.data.parse_string();
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::WSPR_locator)).assert_data();
	cfg.locator = r.msg.data.parse_string();
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::WSPR_reportPower)).assert_data();
	cfg.reportedPower_dBm = r.msg.data.parse_int_le<int8_t>();
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::WSPR_outputPower)).assert_data();
	cfg.outputPower_dBm = r.msg.data.parse_int_le<int8_t>();
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::WSPR_paBias)).assert_data();
	cfg.paBias = r.msg.data.parse_int_le<uint16_t>();
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::WSPR_maxTxDuration)).assert_data();
	cfg.maxRuntime = r.msg.data.parse_int_le<uint64_t>();
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::WSPR_txPct)).assert_data();
	cfg.transmitPercent = r.msg.data.parse_int_le<uint8_t>();
	r = send(DeviceComm::genMsg_read(DeviceComm::VarId::WSPR_txFreq)).assert_data();
	cfg.transmitFreq = r.msg.data.parse_int_le<uint64_t>();

	if (deviceInfo.firmwareVersion.supports_cwId())
	{
		r = send(DeviceComm::genMsg_read(DeviceComm::VarId::CwId_Freq)).assert_data();
		cfg.cwId_freq = r.msg.data.parse_int_le<uint32_t>();
		r = send(DeviceComm::genMsg_read(DeviceComm::VarId::CwId_Callsign)).assert_data();
		cfg.cwId_callsign = r.msg.data.parse_string();
	}
	else
	{
		cfg.cwId_freq = 0;
		cfg.cwId_callsign = "";
	}

	if (deviceInfo.firmwareVersion.supports_varId(DeviceComm::VarId::PaBiasSource))
	{
		r = send(DeviceComm::genMsg_read(DeviceComm::VarId::PaBiasSource)).assert_data();
		cfg.biasSource = (PaBiasSource)r.msg.data.parse_int_le<uint8_t>();
	}
	else
	{
		cfg.biasSource = PaBiasSource::Default;
	}

	if (deviceInfo.firmwareVersion.supports_varId(DeviceComm::VarId::WSPR_optionFlags))
	{
		r = send(DeviceComm::genMsg_read(DeviceComm::VarId::WSPR_optionFlags)).assert_data();
		cfg.optionFlags = r.msg.data.parse_int_le<uint8_t>();
	}
	else
	{
		cfg.optionFlags = 0;
	}

	deviceModel->config = cfg;
	deviceModel->info = deviceInfo;
}

Task_WSPRLoad::~Task_WSPRLoad()
{}
