#ifndef common_Task_Connect_h
#define common_Task_Connect_h

#include "DeviceCommTask.hpp"

class Task_Connect : public DeviceCommTask
{
protected:
	void updateBootloaderState();
public:
	using DeviceCommTask::DeviceCommTask;
	void task() override;
	virtual ~Task_Connect();
};

#endif
