#ifndef common_VirtualFlash32_h
#define common_VirtualFlash32_h

#include <string>
#include <cstdint>
#include <vector>
#include <map>

class VirtualFlash32
{
public:
	std::map<uint32_t, uint8_t> d;
	uint32_t beginAddress, endAddress;

	static uint32_t addr_PA(uint32_t addr)
	{
		// Translate to physical address
		return (addr & 0x1FFFFFFF);
	}
	static uint32_t addr_VA0(uint32_t addr)
	{
		// Translate to KSEG0 address
		return (addr | 0x80000000);
	}
	static uint32_t addr_VA1(uint32_t addr)
	{
		// Translate to KSEG1 address
		return (addr | 0xA0000000);
	}
	void erase()
	{
		beginAddress = 0xFFFFFFFF;
		endAddress = 0;
		d.clear();
	}
	bool isset(uint32_t addr) const
	{
		return d.count(addr_PA(addr));
	}
	uint8_t get(uint32_t addr) const
	{
		auto it = d.find(addr_PA(addr));
		// Erased flash has value 0xFF
		if (it==d.end())
			return 0xFF;
		return it->second;
	}
	void set(uint32_t addr, uint8_t data)
	{
		addr = addr_PA(addr);
		// TODO: move "do not write boot sector" limitation to somewhere better?
		if (addr>=addr_PA(0x9FC00000))
			return;
		if (addr<beginAddress)
			beginAddress = addr;
		if (addr+1>endAddress)
			endAddress = addr+1;
		d[addr] = data;
	}
	template<class T>
	T get_int(uint32_t addr) const
	{
		T x = 0;
		for (uint32_t i=0; i<sizeof(T); i++)
		{
			x |= T(get(addr+i)) << (8*i);
		}
		return x;
	}
	template<class T>
	void set_int(uint32_t addr, T data)
	{
		for (size_t i=0; i<sizeof(T); i++)
		{
			set(addr+i, (data >> (8*i)) & 0xFF);
		}
	}
	void roundBounds();

	VirtualFlash32()
	{
		erase();
	}
};


#endif
