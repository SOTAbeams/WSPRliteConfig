#include "DeviceCommTask.hpp"

DeviceComm::MsgResponse DeviceCommTask::send(DeviceComm::Msg newMsg)
{
	if (!deviceModel || !deviceModel->conn)
	{
		DeviceComm::MsgResponse r;
		r.success = false;
		return r;
	}
	return deviceModel->conn->send(newMsg);
}

DeviceCommTask::DeviceCommTask(std::shared_ptr<DeviceModel> deviceModel_) :
	deviceModel(deviceModel_)
{}

DeviceCommTask::~DeviceCommTask() {}


Task_Msgs_Ack::Task_Msgs_Ack(std::shared_ptr<DeviceModel> deviceModel_) :
	DeviceCommTask(deviceModel_)
{}

Task_Msgs_Ack::~Task_Msgs_Ack()
{}

void Task_Msgs_Ack::task()
{
	for (auto msg: msgs)
	{
		send(msg).assert_ack();
	}
}
