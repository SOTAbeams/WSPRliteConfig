#ifndef Device_h
#define Device_h

#include <queue>
#include <cstdint>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <string>

#include "Serial.hpp"
#include "Version.hpp"
#include "AsyncTaskRunner.hpp"

#include "device/DeviceMode.hpp"
#include "device/MsgType.hpp"


enum class WsprBand
{
	Band_20m=14,
	Band_30m=10,
	Band_40m=7,
	Band_60m=5,
	Band_80m=3,
	Band_160m=1,
};

#define Device_xoFreq_fracBits 4
#define Device_xoFreq_default ((26999985ULL << Device_xoFreq_fracBits) + 3)



namespace DeviceComm
{

class Data
{
public:
	std::vector<uint8_t> d;
	void append(const std::string& newData);
	void append(const uint8_t newData);
	void append(const uint8_t *newData, size_t len);
	void append(const std::vector<uint8_t> newData);
	void append(const char newData);
	void append(const Data& newData);

	template<class T>
	void append_int_le(const T newData)
	{
		for (size_t i=0; i<sizeof(T); i++)
		{
			append(uint8_t((newData>>(8*i))&0xFF));
		}
	}
	template<class T>
	T parse_int_le(size_t startI=0) const
	{
		if (startI+sizeof(T)>d.size())
			throw std::logic_error("Message too short");
		T x = 0;
		for (size_t i=0; i<sizeof(T); i++)
		{
			x |= T(d[startI+i]) << (8*i);
		}
		return x;
	}
	std::string parse_string_bin() const;
	std::string parse_string() const;

	size_t size() const;
	uint8_t *data();
};


class Msg
{
protected:

public:
	MsgType type;
	Data data;
	uint32_t responseTimeout_ms;
	Msg();
	Msg(const Msg&) = default;
	Msg& operator=(const Msg&) = default;
	Msg(Msg&&) = default;
	Msg& operator=(Msg&&) = default;
	Msg(MsgType type_);

	uint32_t calcChecksum();
};

class MsgResponse
{
public:
	Msg msg;
	bool success = false;
	void assert_success() const;
	void assert_msgType(MsgType t) const;
	const MsgResponse& assert_ack() const
	{
		assert_msgType(MsgType::ACK);
		return *this;
	}
	const MsgResponse& assert_data() const
	{
		assert_msgType(MsgType::ResponseData);
		return *this;
	}
	MsgResponse& assert_ack()
	{
		assert_msgType(MsgType::ACK);
		return *this;
	}
	MsgResponse& assert_data() 
	{
		assert_msgType(MsgType::ResponseData);
		return *this;
	}
};

class QueueEntry
{
	using CallbackFunc = std::function<void(MsgResponse const &response)>;
protected:
	std::promise<MsgResponse> responsePromise;
	std::list<CallbackFunc> callbacks;
	std::mutex mtx;
	bool isDone = false;
public:
	Msg msg;
	std::shared_future<MsgResponse> responseFuture;
	QueueEntry();
	void handleResponse(MsgResponse const &response);
	void addCallback(CallbackFunc cb);
	MsgResponse getResponse();
};

class Connection
{
protected:
	static const int defaultTimeoutRx_ms;
	static const int defaultTimeoutTx_ms;
	sp_port *spPort;
	bool connValid;
	std::queue< std::shared_ptr<QueueEntry> > txq;
	std::mutex txqMutex;
	std::recursive_mutex spMutex;
	std::condition_variable txqCV;
	std::thread commThread;
	bool commThreadShouldStop;

	void handleQEntry(std::shared_ptr<QueueEntry> qe);
	bool attemptTx(const Msg &msg);
	bool attemptRx(Msg &response, uint32_t timeoutRx_ms=0);
	std::string getLastErrorMsg();

	void commThreadStart();
	void commThreadStop();
	void commLoop();

public:
	bool isValid() const;
	void connect(SerialPort const &port);
	void disconnect();
	Connection(SerialPort const &port);
	virtual ~Connection();
	// Asynchronous send
	std::shared_ptr<QueueEntry> enqueue(Msg newMsg);
	// Synchronous send (msg is still queued, but function only returns after the msg is replied to)
	MsgResponse send(Msg newMsg);

	std::string getPortDesc() const;
};

Msg genMsg_read(VarId varId);
Msg genMsg_write(VarId varId);
Msg genMsg_write_str(VarId varId, std::string data);

template<class T>
Msg genMsg_write_int(VarId varId, T data)
{
	Msg msg = genMsg_write(varId);
	msg.data.append_int_le<T>(data);
	return msg;
}

}



class DeviceConfig
{
public:
	std::string callsign, locator;
	WsprBand band;
	uint32_t transmitFreq;
	int8_t outputPower_dBm;
	uint16_t paBias;
	int8_t reportedPower_dBm;
	uint8_t transmitPercent;
	int maxRuntime;
	uint64_t xoFreq;

	uint64_t changeCounter;

	uint32_t cwId_freq;
	std::string cwId_callsign;

	static bool isValidCallsign(std::string x);
};

class DeviceAuth
{
public:
	uint64_t id = 0;
	std::vector<uint8_t> secret;
};

class DeviceInfo
{
public:
	DeviceAuth auth;
	FirmwareVersion firmwareVersion;
	DeviceVersion deviceVersion;
	void loadVersionsFromMsg(DeviceComm::Data &data);
};


class DeviceModel
{
public:
	std::shared_ptr<DeviceComm::Connection> conn;
	DeviceConfig config;
	DeviceInfo info;
	uint8_t bootloaderState;
	DeviceMode mode;
	DeviceModeSub submode;

	AsyncTaskRunner taskRunner;
};

#endif

