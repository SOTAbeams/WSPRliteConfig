#include "Task_Connect.hpp"

void Task_Connect::updateBootloaderState()
{
	DeviceComm::MsgResponse r = send(DeviceComm::MsgType::Bootloader_State).assert_data();
	deviceModel->bootloaderState = r.msg.data.parse_int_le<uint8_t>();
}

void Task_Connect::updateVersions()
{
	DeviceInfo deviceInfo = deviceModel->info;

	DeviceComm::MsgResponse r = send(DeviceComm::MsgType::Version).assert_data();
	deviceInfo.loadVersionsFromMsg(r.msg.data);

	deviceModel->info = deviceInfo;
}

void Task_Connect::task()
{
	updateBootloaderState();
	updateVersions();

	if (deviceModel->bootloaderState>1)
	{
		// bootloaderState>1 means the device is in firmware update mode, with no firmware currently present on the device
		return;
	}

	if (deviceModel->bootloaderState==1)
	{
		// bootloaderState==1 means the device is in firmware update mode, but appears to have some firmware present which we should be able to reset the device to boot into
		send(DeviceComm::MsgType::Reset);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		updateBootloaderState();
		if (deviceModel->bootloaderState==1)
		{
			throw std::runtime_error("Device is in firmware update mode, settings cannot currently be changed. Please disconnect and reconnect the USB cable then try again.");
		}
		updateVersions();
	}
}

Task_Connect::~Task_Connect()
{}
