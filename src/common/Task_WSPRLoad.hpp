#ifndef common_Task_WSPRLoad_h
#define common_Task_WSPRLoad_h

#include "common/DeviceCommTask.hpp"

class Task_WSPRLoad : public DeviceCommTask
{
public:
	using DeviceCommTask::DeviceCommTask;
	void task() override;
	virtual ~Task_WSPRLoad();
};

#endif
