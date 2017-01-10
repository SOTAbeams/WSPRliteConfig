#ifndef common_Task_FirmwareUpdate_h
#define common_Task_FirmwareUpdate_h

#include "DeviceCommTask.hpp"
#include "HexFile.hpp"

class Task_FirmwareUpdate : public DeviceCommTask
{
protected:
	void erase();
	void program();
	void verify();
	void writeResetAddress();
public:
	using DeviceCommTask::DeviceCommTask;
	HexFile fwHex;
	void task() override;
	virtual ~Task_FirmwareUpdate();
};

#endif
