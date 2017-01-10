#include "Serial.hpp"
#include <libserialport.h>
#include <stdexcept>
#include "StrUtil.hpp"

#if SP_PACKAGE_VERSION_MAJOR<=0 && SP_PACKAGE_VERSION_MINOR<=1 && SP_PACKAGE_VERSION_MICRO<1
#error libserialport version is too old
#endif

SerialPortsList::SerialPortsList()
{
	Update();
}

SerialPortsList::~SerialPortsList()
{}

void SerialPortsList::Update()
{
	std::vector<SerialPort> newPorts;
	
	sp_return ret;
	sp_port **portsPtr;
	ret = sp_list_ports(&portsPtr);

	if (ret==SP_OK && portsPtr!=nullptr)
	{
		for (sp_port **p=portsPtr; *p!=nullptr; p++)
		{
			newPorts.push_back(SerialPort(*p));
		}
	}
	ports = std::move(newPorts);

	sp_free_port_list(portsPtr);
}



SerialPort::SerialPort(sp_port const *port)
{
	sp_return ret = sp_copy_port(port, &spPort);
	if (ret!=SP_OK)
	{
		throw std::runtime_error("Error obtaining serial port information");
	}
}

SerialPort::SerialPort(const SerialPort &x) : SerialPort(x.spPort) {}

SerialPort::~SerialPort()
{
	sp_free_port(spPort);
}

std::string SerialPort::GetName() const
{
	return sp_get_port_name(spPort);
}

std::string SerialPort::GetDesc() const
{
	const char* txt = sp_get_port_usb_product(spPort);
	if (!txt || !std::string(txt).length())
		txt = sp_get_port_description(spPort);
	if (!txt)
		return "";
	return txt;
}

const sp_port *SerialPort::GetSpPort() const
{
	return spPort;
}

sp_port *SerialPort::GetSpPort()
{
	return spPort;
}
