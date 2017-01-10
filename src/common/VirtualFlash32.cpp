#include "VirtualFlash32.hpp"


void VirtualFlash32::roundBounds()
{
	// Expand min/max address to align to 32 bits
	beginAddress -= beginAddress % 4;
	endAddress += endAddress % 4;
}
