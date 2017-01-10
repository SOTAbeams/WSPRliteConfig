#ifndef common_HexFile_h
#define common_HexFile_h

#include "VirtualFlash32.hpp"
#include "Version.hpp"

#include <fstream>
#include <string>


#define BOOT_SECTOR_BEGIN 0x9FC00000
#define APPLICATION_START 0x9D006000
#define APP_RESET_ADDRESS (APPLICATION_START + 0x1000)


class HexRecord 
{
public:
	unsigned char RecDataLen;
	unsigned int Address;
	unsigned int MaxAddress;
	unsigned int MinAddress;
	unsigned char RecType;
	unsigned char* Data;
	unsigned char CheckSum;	
	unsigned int ExtSegAddress;
	unsigned int ExtLinAddress;
};

class HexFileReader
{
protected:
	std::ifstream f;
public:
	uint32_t currentLine;
	uint32_t lineCount;
	void reset();
	std::string getline();
	std::vector<uint8_t> getDecodedLine();
	float getProgress();
	bool eof();

	HexFileReader(std::string filename);
	virtual ~HexFileReader();
};

class HexFileChecksum
{
public:
	uint16_t crc;
	uint32_t startAddress, progLength;
};

class HexFile
{
protected:
public:
	std::string hexFilename;
	VirtualFlash32 flash;

	void parse();
	void load(std::string hexFilename_);

	HexFileChecksum getChecksum();
	FirmwareVersion getVersion();
	uint32_t getResetAddressData();

	virtual ~HexFile();
};


#endif
