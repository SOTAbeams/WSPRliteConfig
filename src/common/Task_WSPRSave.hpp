#ifndef common_Task_WSPRSave_h
#define common_Task_WSPRSave_h

#include "common/DeviceCommTask.hpp"
#include "common/Device.hpp"

class Task_WSPRSave : public DeviceCommTask
{
public:
	using DeviceCommTask::DeviceCommTask;
	DeviceConfig newCfg;
	void task() override;
	virtual ~Task_WSPRSave();
};

#endif
