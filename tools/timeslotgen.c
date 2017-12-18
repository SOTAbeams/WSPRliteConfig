#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*
This program will print the sequence of transmit timeslots resulting from a particular repeat rate setting.

Note: there is a bug in older versions of the firmware (v1.1.3 and older) that means for higher repeat rate settings, fewer timeslots than required are selected.
This has been corrected in newer releases of the firmware, so the resulting sequence of transmit timeslots will be different.
This program contains the new timeslot selection code. To replicate the old buggy behaviour, find and delete the "freeSpaces--;" line in this file.

Repeat rate and number of timeslots are passed as arguments when running this program - see printUsage() below.
The output is one line per timeslot: time formatted as HH:MM, then "1" to indicate that the WSPRlite will transmit in that timeslot or "0" if no transmission.
Example:

  Time  Transmit?
  0:00  1
  0:02  0
  0:04  0
  0:06  0
  0:08  1
  0:10  0
  0:12  0

 */

void printUsage(int argc, char *argv[])
{
	printf("Usage: %s <repeatRate> <count>\n"
	"repeatRate is an integer percentage, 0-100 (the config program limits this to 1-50).\n"
	"count is how many timeslots to print.\n",
	(argc>0) ? argv[0] : "");
}



// The RNG is based on the JKISS code in http://www0.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf
// Claimed period approx 2^127

// RNG state
typedef struct
{
	uint32_t x, y, z, c;
} rng_t;

extern rng_t mainRng;

void rng_init(rng_t *r);

// Return a random uint32
uint32_t rng_uint32(rng_t *r);

// Return a random uint32 in the range [0,maxVal) - includes 0, excludes maxVal
uint32_t rng_uint32_max(rng_t *r, uint32_t maxVal);
// Chance, fraction=n/d
bool rng_chance(rng_t *r, uint32_t n, uint32_t d);


void rng_init(rng_t *r)
{
	// Some initial values from the JKISS document
	r->x = 123456789;
	r->y = 987654321;
	r->z = 43219876;
	r->c = 6543217;
}


uint32_t rng_uint32(rng_t *r)
{
	uint_fast64_t t;
	r->x = 314527869 * r->x + 1234567;
	r->y ^= r->y << 5; r->y ^= r->y >> 7; r->y ^= r->y << 22;
	t = 4294584393ULL * r->z + r->c; r->c = t >> 32; r->z = t;
	uint32_t result = r->x+r->y+r->z;
	return result;
}

uint32_t rng_uint32_max(rng_t *r, uint32_t maxVal)
{
	return rng_uint32(r) % maxVal;
}

bool rng_chance(rng_t *r, uint32_t n, uint32_t d)
{
	return (n>0 && rng_uint32_max(r, d) < n);
}

// Allocates a batch of transmit slots, returning them as a bitmask (aligned to LSB)
uint32_t tslots_randomAlloc_gen(rng_t *rng, int txPercent)
{
	int batchSize = 5;
	uint32_t txSlots = 0;

	// Calculate number of transmit slots to allocate in this batch
	// It will be either floor(batchSize*txPercent/100) or ceil(batchSize*txPercent/100), depending on a random chance
	div_t txSlotsDiv = div(batchSize*txPercent, 100);
	int txSlotCount = txSlotsDiv.quot;
	if (rng_chance(rng, txSlotsDiv.rem, 100))
		txSlotCount++;
	if (txSlotCount>batchSize)
		txSlotCount = batchSize;

	uint32_t freeSpaces = batchSize;
	// Randomly select transmit slots within the batch until there are enough
	while (txSlotCount>0)
	{
		uint_fast8_t selectedSlot = rng_uint32_max(rng, freeSpaces);
		uint_fast8_t bitIndex, i=0;
		for (bitIndex=0; bitIndex<batchSize; bitIndex++)
		{
			if (txSlots & (1<<bitIndex))
			{
				continue;
			}
			if (i==selectedSlot)
			{
				txSlots |= (1<<bitIndex);
				break;
			}
			i++;
		}
		txSlotCount--;
		// Note: in old firmware versions, the following line was missing, which occasionally caused there to be fewer timeslots than required
		freeSpaces--;
	}
	return txSlots;
}

void printSlot(uint64_t i, int value)
{
	unsigned long long m = i*2 % 60;
	unsigned long long h = i*2 / 60;
	printf("%3llu:%02llu  %d\n", h, m, value);
}

int main (int argc, char *argv[])
{
	uint32_t repeatRate = 20;
	uint64_t count = 0;
	if (argc <= 2)
	{
		// Error, not enough arguments
		printUsage(argc, argv);
		return 1;
	}

	// Parse arguments
	if (sscanf(argv[1], "%lu", &repeatRate)!=1 || repeatRate>100)
	{
		printUsage(argc, argv);
		return 1;
	}
	if (sscanf(argv[2], "%llu", &count)!=1)
	{
		printUsage(argc, argv);
		return 1;
	}

	uint32_t tslots = 0;
	rng_t rng;
	rng_init(&rng);

	// Print header
	puts("  Time  Transmit?");

	// First timeslot is always a transmission. Subsequent timeslots are generated randomly in batches of 5.
	if (count>=1)
	{
		printSlot(0, 1);
	}
	for (uint64_t i=1; i<count; i++)
	{
		int batchI = (i-1)%5;
		if (batchI==0)
		{
			tslots = tslots_randomAlloc_gen(&rng, repeatRate);
		}
		printSlot(i, (tslots>>batchI)&1);
	}
}


