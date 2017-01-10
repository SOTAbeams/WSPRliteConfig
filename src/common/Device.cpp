#include "Device.hpp"
#include <libserialport.h>
#include <array>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <regex>
#include "StrUtil.hpp"
#include "crc.hpp"

namespace DeviceComm
{

#define cmdconn_START ('\x01')
#define cmdconn_END ('\x04')
#define cmdconn_ESC ('\x10')

const int Connection::defaultTimeoutRx_ms = 100;
const int Connection::defaultTimeoutTx_ms = 100;

void Connection::handleQEntry(std::shared_ptr<QueueEntry> qe)
{
	MsgResponse response;
	for (int i=0; i<3; i++)
	{
		if (!attemptTx(qe->msg))
		{
			std::cerr << "Communication failed: tx" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(defaultTimeoutTx_ms));
			continue;
		}
		response.success = attemptRx(response.msg, qe->msg.responseTimeout_ms);
		if (response.success)
		{
			break;
		}
		else
		{
			std::cerr << "Communication failed: rx" << std::endl;
		}
	}
	qe->handleResponse(response);
}

bool Connection::attemptTx(const Msg &msg)
{
	std::lock_guard<std::recursive_mutex> lksp(spMutex);
	if (!connValid)
		return false;

	// Empty receive buffer, just in case the device has transmitted unexpected responses to previous commands
	if (sp_input_waiting(spPort)>0)
	{
		std::array<uint8_t, 128> data;
		sp_nonblocking_read(spPort, data.data(), 128);
	}

	// Turn the Msg into a sequence of bytes to transmit
	Data txData;
	uint16_t msgType = (uint16_t)msg.type;
	txData.append(uint8_t(msgType&0xFF));
	txData.append(uint8_t((msgType>>8)&0xFF));
	txData.append(msg.data);
	txData.append_int_le<uint32_t>(CRC::Calculate(txData.data(), txData.size(), CRC::CRC_32()));

	Data txDataEsc;
	txDataEsc.append(cmdconn_START);
	for (uint8_t c: txData.d)
	{
		if (c==cmdconn_START || c==cmdconn_END || c==cmdconn_ESC)
		{
			txDataEsc.append(cmdconn_ESC);
			txDataEsc.append(uint8_t(c + 0x80));
		}
		else
		{
			txDataEsc.append(c);
		}
	}
	txDataEsc.append(cmdconn_END);

	// Write data to serial port
	int ret = sp_blocking_write(spPort, txDataEsc.data(), txDataEsc.size(), defaultTimeoutTx_ms);
	if (ret==txDataEsc.size())
	{
		return true;
	}
	return false;
}

enum class RxState
{
	WaitForStart,
	RxMsg,
	RxMsgEscaped,
};


bool Connection::attemptRx(Msg &response, uint32_t timeoutRx_ms)
{
	Msg rxMsg;
	RxState state = RxState::WaitForStart;
	auto rxStartTime = std::chrono::steady_clock::now();
	uint32_t msgRxI = 0;
	uint16_t msgType = 0;
	if (timeoutRx_ms==0)
		timeoutRx_ms = defaultTimeoutRx_ms;
	while (1)
	{
		if (std::chrono::steady_clock::now() > rxStartTime + std::chrono::milliseconds(timeoutRx_ms))
			return false;
		std::lock_guard<std::recursive_mutex> lksp(spMutex);
		if (!connValid)
			return false;

		int bytesWaiting = sp_input_waiting(spPort);
		if (bytesWaiting>0)
		{
			char rxTmpBuffer[bytesWaiting];
			sp_nonblocking_read(spPort, rxTmpBuffer, bytesWaiting);
			for (int b=0; b<bytesWaiting; b++)
			{
				uint8_t c = rxTmpBuffer[b];
				if (state==RxState::WaitForStart)
				{
					if (c==cmdconn_START)
					{
						state = RxState::RxMsg;
						rxMsg.data.d.clear();
						msgRxI = 0;
						msgType = 0;
					}
					continue;
				}
				else if (state==RxState::RxMsg || state==RxState::RxMsgEscaped)
				{
					if (state==RxState::RxMsgEscaped)
					{
						state = RxState::RxMsg;
						if (c<0x80)
							continue;
						c -= 0x80;
					}
					else
					{
						if (c==cmdconn_ESC)
						{
							state = RxState::RxMsgEscaped;
							continue;
						}
						if (c==cmdconn_END)
						{
							rxMsg.type = (MsgType)msgType;
							if (msgRxI>=2 && rxMsg.data.size()>=sizeof(uint32_t))
							{
								uint32_t csumRx, csumActual;
								csumRx = rxMsg.data.parse_int_le<uint32_t>(rxMsg.data.size()-sizeof(uint32_t));
								rxMsg.data.d.resize(rxMsg.data.d.size()-sizeof(uint32_t));
								csumActual = rxMsg.calcChecksum();

								if (csumRx==csumActual)
								{
									state = RxState::WaitForStart;
									response = rxMsg;
									return true;
								}
								else
								{
									std::cerr << "Incorrect serial message checksum" << std::endl;
								}
							}
							state = RxState::WaitForStart;
							continue;
						}
						if (c==cmdconn_START)
						{
							rxMsg.data.d.clear();
							msgRxI = 0;
							msgType = 0;
							continue;
						}
					}
					if (msgRxI<2)
					{
						msgType |= ((uint16_t)(uint8_t)c)<<(msgRxI);
					}
					else
					{
						rxMsg.data.append(c);
					}
					msgRxI++;
				}
			}
		}
	}
	return false;
}

std::string Connection::getLastErrorMsg()
{
	std::lock_guard<std::recursive_mutex> lksp(spMutex);
	char* msg = sp_last_error_message();
	std::string msgStr = msg;
	sp_free_error_message(msg);
	return msgStr;
}


void Connection::commThreadStart()
{
	commThreadStop();

	{
		std::lock_guard<std::mutex> lk(txqMutex);
		commThreadShouldStop = false;
	}
	commThread = std::thread([this](){
		commLoop();
	});
}

void Connection::commThreadStop()
{
	if (commThread.joinable())
	{
		{
			std::lock_guard<std::mutex> lk(txqMutex);
			commThreadShouldStop = true;
		}
		txqCV.notify_all();
		commThread.join();
	}
}

void Connection::commLoop()
{
	while (!commThreadShouldStop)
	{
		std::shared_ptr<QueueEntry> qe;
		bool msgWaiting = false;

		{
			std::lock_guard<std::mutex> lk(txqMutex);
			if (commThreadShouldStop)
			{
				break;
			}
			if (!txq.empty())
			{
				qe = txq.front();
				txq.pop();
				msgWaiting = true;
			}
		}

		if (msgWaiting)
		{
			handleQEntry(qe);
		}
		else
		{
			std::unique_lock<std::mutex> lk(txqMutex);
			txqCV.wait(lk, [this](){
				return (commThreadShouldStop || !txq.empty());
			});
		}
	}
}

bool Connection::isValid() const
{
	return connValid;
}

void Connection::connect(const SerialPort &port)
{
	std::lock_guard<std::recursive_mutex> lksp(spMutex);
	if (connValid)
		disconnect();

	sp_return ret;
	ret = sp_copy_port(port.GetSpPort(), &spPort);
	if (ret!=SP_OK)
		throw std::runtime_error("Error opening serial port: could not copy sp_port");
	ret = sp_open(spPort, SP_MODE_READ_WRITE);
	if (ret!=SP_OK)
	{
		char *err = sp_last_error_message();
		std::string msg = std::string("Error opening serial port: sp_open failed. ") + err;
		sp_free_error_message(err);
		throw std::runtime_error(msg);
	}

	ret = sp_set_baudrate(spPort, 1000000);
	if (ret!=SP_OK)
		throw std::runtime_error("Error opening serial port: sp_set_baudrate failed");
	ret = sp_set_bits(spPort, 8);
	if (ret!=SP_OK)
		throw std::runtime_error("Error opening serial port: sp_set_bits failed");
	ret = sp_set_parity(spPort, SP_PARITY_NONE);
	if (ret!=SP_OK)
		throw std::runtime_error("Error opening serial port: sp_set_parity failed");
	ret = sp_set_stopbits(spPort, 2);
	if (ret!=SP_OK)
		throw std::runtime_error("Error opening serial port: sp_set_stopbits failed");
	ret = sp_set_flowcontrol(spPort, SP_FLOWCONTROL_RTSCTS);
	if (ret!=SP_OK)
		throw std::runtime_error("Error opening serial port: sp_set_flowcontrol failed");

	connValid = true;
}

void Connection::disconnect()
{
	std::lock_guard<std::recursive_mutex> lksp(spMutex);
	sp_close(spPort);
	connValid = false;
}

Connection::Connection(const SerialPort &port) : connValid(false), spPort(nullptr), commThreadShouldStop(false)
{
	connect(port);
	commThreadStart();
}

Connection::~Connection()
{
	disconnect();
	commThreadStop();
}

std::shared_ptr<QueueEntry> Connection::enqueue(Msg newMsg)
{
	auto qEntry = std::make_shared<QueueEntry>();
	qEntry->msg = newMsg;

	{
		std::lock_guard<std::mutex> lk(txqMutex);
		txq.push(qEntry);
	}
	txqCV.notify_all();

	return qEntry;
}

MsgResponse Connection::send(Msg newMsg)
{
	std::shared_ptr<QueueEntry> qe = enqueue(newMsg);
	return qe->getResponse();
}

std::string Connection::getPortDesc() const
{
	if (spPort)
		return SerialPort(spPort).GetDesc();
	return "";
}

QueueEntry::QueueEntry()
{
	responseFuture = responsePromise.get_future().share();
}

void QueueEntry::handleResponse(const MsgResponse &response)
{
	responsePromise.set_value(response);
	std::list<CallbackFunc>::iterator it;
	bool isFirst = true;
	bool callbacksDone = false;
	while (!callbacksDone)
	{
		{
			std::lock_guard<std::mutex> lk(mtx);
			if (isFirst)
				it = callbacks.begin();
			else
				++it;
			isFirst = false;
			if (it==callbacks.end())
			{
				isDone = true;
				return;
			}
		}
		(*it)(response);
	}
}

void QueueEntry::addCallback(QueueEntry::CallbackFunc cb)
{
	{
		std::lock_guard<std::mutex> lk(mtx);
		if (!isDone)
		{
			callbacks.push_back(std::move(cb));
			return;
		}
	}
	cb(responseFuture.get());
}

MsgResponse QueueEntry::getResponse()
{
	responseFuture.wait();
	return responseFuture.get();
}



void Data::append(const std::string& newData)
{
	std::copy(newData.begin(), newData.end(), std::back_inserter(d));
}

void Data::append(const uint8_t newData)
{
	d.push_back(newData);
}

void Data::append(const uint8_t *newData, size_t len)
{
	std::copy(newData, newData+len, std::back_inserter(d));
}

void Data::append(const std::vector<uint8_t> newData)
{
	std::copy(newData.begin(), newData.end(), std::back_inserter(d));
}

void Data::append(const char newData)
{
	d.push_back(newData);
}

void Data::append(const Data &newData)
{
	std::copy(newData.d.begin(), newData.d.end(), std::back_inserter(d));
}

std::string Data::parse_string_bin() const
{
	std::string x;
	std::copy(d.begin(), d.end(), std::back_inserter(x));
	return x;
}

std::string Data::parse_string() const
{
	std::string x;
	for (size_t i=0; i<d.size(); i++)
	{
		if (d[i]==0)
			break;
		x.push_back(d[i]);
	}
	return x;
}

size_t Data::size() const
{
	return d.size();
}

uint8_t *Data::data()
{
	return d.data();
}


Msg genMsg_read(VarId varId)
{
	Msg msg;
	msg.type = MsgType::Read;
	uint16_t varId_tmp = (uint16_t)varId;
	msg.data.append(uint8_t(varId_tmp&0xFF));
	msg.data.append(uint8_t((varId_tmp>>8)&0xFF));
	return msg;
}

Msg genMsg_write(VarId varId)
{
	Msg msg;
	msg.type = MsgType::Write;
	uint16_t varId_tmp = (uint16_t)varId;
	msg.data.append(uint8_t(varId_tmp&0xFF));
	msg.data.append(uint8_t((varId_tmp>>8)&0xFF));
	return msg;
}

Msg genMsg_write_str(VarId varId, std::string data)
{
	Msg msg = genMsg_write(varId);
	msg.data.append(data);
	return msg;
}

Msg::Msg() : responseTimeout_ms(0)
{}

Msg::Msg(MsgType type_) : type(type_), responseTimeout_ms(0)
{}

uint32_t Msg::calcChecksum()
{
	Data csumData;
	csumData.append_int_le<uint16_t>((uint16_t)type);
	csumData.append(data);
	return CRC::Calculate(csumData.data(), csumData.size(), CRC::CRC_32());
}

void MsgResponse::assert_msgType(MsgType t) const
{
	assert_success();
	if (t!=DeviceComm::MsgType::NACK && msg.type==DeviceComm::MsgType::NACK)
	{
		std::string errMsg = msg.data.parse_string();
		throw std::runtime_error("Error reading from device: " + errMsg);
	}
	if (msg.type!=t)
	{
		throw std::runtime_error("Error reading from device: unexpected response type.");
	}
}

void MsgResponse::assert_success() const
{
	if (!success)
	{
		throw std::runtime_error("Communication with device failed - no response received.");
		return;
	}
}



}



bool DeviceConfig::isValidCallsign(std::string x)
{
	std::regex validCallsignRegex("^[A-Z0-9 ]?[A-Z0-9][0-9][A-Z ]{0,3}$");
	return std::regex_match(x, validCallsignRegex);
}


void DeviceInfo::loadVersionsFromMsg(DeviceComm::Data &data)
{
	deviceVersion.loadFromMsgData(data, 0);
	firmwareVersion.loadFromMsgData(data, sizeof(uint32_t)*3);
}
