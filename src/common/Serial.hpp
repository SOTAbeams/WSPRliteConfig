#ifndef Serial_h
#define Serial_h

#include <string>
#include <vector>


// This is a C++ wrapper around the libserialport library


class sp_port;

class SerialPort
{
protected:
	sp_port *spPort;
public:
	SerialPort(sp_port const *port);
	SerialPort(SerialPort const &x);
	virtual ~SerialPort();

	std::string GetName() const;
	std::string GetDesc() const;
	sp_port const * GetSpPort() const;
	sp_port * GetSpPort();
};

class SerialPortsList
{
public:
	std::vector<SerialPort> ports;
	SerialPortsList();
	virtual ~SerialPortsList();
	void Update();
};

#endif
