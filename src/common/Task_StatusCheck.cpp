#include "Task_StatusCheck.hpp"

void Task_StatusCheck::updateBootloaderState()
{
	DeviceComm::MsgResponse r = send(DeviceComm::MsgType::Bootloader_State).assert_data();
	deviceModel->bootloaderState = r.msg.data.parse_int_le<uint8_t>();
}

void Task_StatusCheck::task()
{
	DeviceComm::MsgResponse r;

	updateBootloaderState();
	if (deviceModel->bootloaderState>0)
	{
		return;
	}

	if (deviceModel->info.firmwareVersion.supports_msg(DeviceComm::MsgType::DeviceMode_Get))
	{
		DeviceComm::MsgResponse r = send(DeviceComm::MsgType::DeviceMode_Get).assert_data();
		deviceModel->mode = (DeviceMode)r.msg.data.parse_int_le<uint16_t>(0);
		if (deviceModel->mode == DeviceMode::WSPR_Active)
		{
			deviceModel->submode.wspr = (DeviceModeSub_WSPR)r.msg.data.parse_int_le<uint8_t>(2);
		}
	}
}

Task_StatusCheck::~Task_StatusCheck()
{}
