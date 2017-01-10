#ifndef common_Task_StatusCheck_h
#define common_Task_StatusCheck_h

#include "DeviceCommTask.hpp"

class Task_StatusCheck : public DeviceCommTask
{
protected:
	void updateBootloaderState();
public:
	using DeviceCommTask::DeviceCommTask;
	void task() override;
	virtual ~Task_StatusCheck();
};

#endif
