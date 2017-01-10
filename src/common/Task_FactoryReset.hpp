#ifndef common_Task_FactoryReset_h
#define common_Task_FactoryReset_h

#include "common/DeviceCommTask.hpp"

class Task_FactoryReset : public DeviceCommTask
{
public:
	using DeviceCommTask::DeviceCommTask;
	void task() override;
	virtual ~Task_FactoryReset();
};

#endif
