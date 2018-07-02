#ifndef common_WsprMsgTypes_h
#define common_WsprMsgTypes_h

/*
 * WSPR has the following message types:
 * Type 1: K1ABC FN42 37
 *   Basic callsign, 4 char locator, power
 * Type 2: PJ4/K1ABC 37
 *   Compound callsign, power
 * Type 3: <PJ4/K1ABC> FK52UD 37
 *   Callsign hash, 6 char locator, power
 *
 *
 * If the firmware supports type 2 and 3 messages, it will select which
 * types of message to transmit based on the WSPR settings:
 *  - If a compound callsign is being used, type 2 and 3.
 *  - If a basic callsign is being used, the locator is at least 6 chars long, and the "force extended WSPR" flag is on, type 1 and 3.
 *  - Otherwise, type 1 only.
 */

enum class WsprMsgTypes
{
	None,
	Type1,
	Type1_3,
	Type2_3,
};

#endif
