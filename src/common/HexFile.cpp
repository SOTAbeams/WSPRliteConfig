#include "HexFile.hpp"
#include "StrUtil.hpp"
#include "crc.hpp"
#include "Device.hpp"


void HexFileReader::reset()
{
	f.clear();
	f.seekg(0);
	currentLine = 0;
}

std::string HexFileReader::getline()
{
	std::string line;
	std::getline(f, line);
	currentLine++;
	return line;
}

std::vector<uint8_t> HexFileReader::getDecodedLine()
{
	while (!eof())
	{
		std::string line = getline();
		if (line[0]==':')
			return StrUtil::hex_decode(line.substr(1));
	}
	return std::vector<uint8_t>();
}

float HexFileReader::getProgress()
{
	return (float)currentLine/lineCount;
}

bool HexFileReader::eof()
{
	return f.eof();
}

HexFileReader::HexFileReader(std::string filename)
{
	f.open(filename);
	reset();
	while (!eof())
		getline();
	lineCount = currentLine;
	reset();
}

HexFileReader::~HexFileReader()
{}


#define DATA_RECORD 		0
#define END_OF_FILE_RECORD 	1
#define EXT_SEG_ADRS_RECORD 2
#define EXT_LIN_ADRS_RECORD 4

void HexFile::parse()
{
	uint32_t address;
	uint32_t segAddress=0, linAddress=0;
	HexFileReader r(hexFilename);
	while (!r.eof())
	{
		std::vector<uint8_t> lineData = r.getDecodedLine();
		if (lineData.size()<5)
			continue;
		uint8_t dataLen = lineData[0];
		uint8_t recType = lineData[3];
		uint8_t *data = lineData.data()+4;

		switch(recType)
		{
			case DATA_RECORD:
				address = linAddress + segAddress;
				address += (lineData[1] << 8) | (lineData[2]);
				for (size_t i=0; i<dataLen; i++)
					flash.set(address+i, data[i]);
				break;
			case EXT_SEG_ADRS_RECORD:
				segAddress = (data[0] << 16) | (data[1] << 8);
				linAddress = 0;
				break;
					
			case EXT_LIN_ADRS_RECORD:
				linAddress = (data[0] << 24) | (data[1] << 16);
				segAddress = 0;
				break;
			case END_OF_FILE_RECORD:
			default: 
				segAddress = 0;
				linAddress = 0;
				break;
		}	
	}
	flash.roundBounds();
}

void HexFile::load(std::string hexFilename_)
{
	hexFilename = hexFilename_;
	parse();
}

HexFileChecksum HexFile::getChecksum()
{
	HexFileChecksum csum;
	csum.startAddress = VirtualFlash32::addr_VA0(flash.beginAddress);
	uint32_t progLen = flash.endAddress-flash.beginAddress;
	csum.progLength = progLen;

	if (flash.beginAddress>=flash.endAddress)
	{
		csum.progLength = 0;
		csum.crc = 0xFFFF;
		return csum;
	}

	/* Temporarily clear app reset address - the bootloader on the PIC will avoid
	 * writing it until a special command is given. The app reset address is not
	 * written until after verification, so calculation of the checksum for
	 * verification should be with this memory location erased.
	 */
	uint32_t resetAddressData = flash.get_int<uint32_t>(APP_RESET_ADDRESS);
	flash.set_int<uint32_t>(APP_RESET_ADDRESS, 0xFFFFFFFF);

	uint8_t *data = new uint8_t[progLen];
	for (uint32_t i=0; i<progLen; i++)
		data[i] = flash.get(flash.beginAddress+i);

	csum.crc = CRC::Calculate(data, progLen, CRC::CRC_16_XMODEM());
	delete[] data;

	flash.set_int<uint32_t>(APP_RESET_ADDRESS, resetAddressData);
	return csum;
}

FirmwareVersion HexFile::getVersion()
{
	FirmwareVersion v;
	DeviceComm::Data d;

	uint32_t startAddr = APPLICATION_START+0x2000;
	for (uint32_t i=0; i<sizeof(uint32_t)*4; i++)
		d.append(flash.get(startAddr+i));

	v.loadFromMsgData(d);
	return v;
}

uint32_t HexFile::getResetAddressData()
{
	return flash.get_int<uint32_t>(APP_RESET_ADDRESS);
}

HexFile::~HexFile()
{}

