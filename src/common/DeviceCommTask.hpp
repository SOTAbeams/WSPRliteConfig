#ifndef common_DeviceCommTask_h
#define common_DeviceCommTask_h

#include "AsyncTask.hpp"
#include "Device.hpp"

#include <vector>

class DeviceCommTask : public AsyncTask
{
protected:
	DeviceComm::MsgResponse send(DeviceComm::Msg newMsg);
public:
	std::shared_ptr<DeviceModel> deviceModel;
	DeviceCommTask(std::shared_ptr<DeviceModel> deviceModel_);
	virtual ~DeviceCommTask();
};

class Task_Msgs_Ack : public DeviceCommTask
{
public:
	std::vector<DeviceComm::Msg> msgs;
	Task_Msgs_Ack(std::shared_ptr<DeviceModel> deviceModel_);
	virtual ~Task_Msgs_Ack();
	void task() override;
};

#endif
