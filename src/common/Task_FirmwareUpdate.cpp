#include "common/Task_FirmwareUpdate.hpp"
#include "common/HexFile.hpp"

void Task_FirmwareUpdate::erase()
{
	DeviceComm::Msg msg(DeviceComm::MsgType::Bootloader_EraseAll);
	msg.responseTimeout_ms = 60000;
	send(msg).assert_ack();
}

void Task_FirmwareUpdate::program()
{
	HexFileReader hexr(fwHex.hexFilename);
	hexr.reset();
	while (!hexr.eof())
	{
		std::vector<uint8_t> lineData = hexr.getDecodedLine();
		if (!lineData.size())
			continue;
		DeviceComm::Msg msg;
		msg.type = DeviceComm::MsgType::Bootloader_ProgramHexRec;
		msg.data.append(lineData);
		send(msg).assert_ack();

		if (hexr.currentLine%10==0)
		{
			onProgress(hexr.getProgress()*0.99, "Programming");
		}
	}
}

void Task_FirmwareUpdate::verify()
{
	HexFileChecksum csum = fwHex.getChecksum();

	DeviceComm::Msg msg;
	msg.type = DeviceComm::MsgType::Bootloader_CRC;
	msg.responseTimeout_ms = 5000;
	msg.data.append_int_le<uint32_t>(csum.startAddress);
	msg.data.append_int_le<uint32_t>(csum.progLength);
	DeviceComm::MsgResponse r = send(msg);
	r.assert_data();
	uint16_t actualCRC = r.msg.data.parse_int_le<uint16_t>();
	if (actualCRC!=csum.crc)
	{
		throw std::runtime_error("Verification failed");
	}
}

void Task_FirmwareUpdate::writeResetAddress()
{
	DeviceComm::Msg msg;
	msg.type = DeviceComm::MsgType::Bootloader_ProgramResetAddr;
	uint32_t resetAddressData = fwHex.getResetAddressData();
	msg.data.append_int_le<uint32_t>(resetAddressData);
	send(msg).assert_ack();
}

void Task_FirmwareUpdate::task()
{
	onProgress(0, "Entering bootloader mode");
	send(DeviceComm::Msg(DeviceComm::MsgType::Bootloader_Enter)).assert_ack();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	{
		DeviceComm::MsgResponse r = send(DeviceComm::MsgType::Bootloader_State);
		r.assert_data();
		if (!r.msg.data.parse_int_le<uint8_t>())
		{
			throw std::runtime_error("Failed to enter bootloader mode");
		}
	}

	onProgress(0, "Erasing (may take a few seconds)");
	erase();
	onProgress(0, "Programming");
	program();
	onProgress(0.99, "Verifying");
	verify();
	onProgress(0.99, "Writing reset address");
	writeResetAddress();

	onProgress(0.99, "Resetting");
	send(DeviceComm::Msg(DeviceComm::MsgType::Reset)).assert_ack();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	onProgress(1, "Done");
}

Task_FirmwareUpdate::~Task_FirmwareUpdate()
{}

