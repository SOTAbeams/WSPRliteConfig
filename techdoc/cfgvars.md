
| ID | Variable name | Type | Purpose |
|----|---------------|------|---------|
| 0  | MemVersion | uint64 | Version number for layout of variables in EEPROM. This should only normally be written by the firmware, not by the config app. |
| 1  | xoFreq | uint64 | Oscillator frequency. This allows frequency calibration to be performed. However, the frequency consistency between boards is good enough that they don't really need any calibration. This number is in Hz and has 4 fractional bits (stored number is 2^4 times the actual frequency). Older firmware revisions ignore this and use a hardcoded frequency.|
| 2  | xoFreqFactory | uint64 | DEPRECATED. Oscillator frequency, see xoFreq. This was designed to be a safe reset value for xoFreq if the user made a mistake while trying to calibrate it. However, a hardcoded value will suffice for that purpose, so this variable may be removed in the future. |
| 3  | ChangeCounter | uint64 | Counts how many times the callsign has been modified. This should be incremented by the config app every time it changes the callsign variable. |
| 4  | DeviceId | uint64 | Used to allow premium access to dxplorer.net. Acts like a username. Should not be modified by the config program. |
| 5  | DeviceSecret | binary, 64 bytes | Used to allow premium access to dxplorer.net. Acts like a password, and should not be published. Should not be modified by the config program. See src/common/dxplorer.cpp (DXplorer::generateKey) for details. |
| 6  | WSPR_txFreq | uint64 | Desired transmission frequency for WSPR. Measured in Hz, no fractional bits. |
| 7  | WSPR_locator | string, 8 bytes | Maidenhead grid locator to use in WSPR messages.  |
| 8  | WSPR_callsign | string, 15 bytes | Callsign to use in WSPR messages. |
| 9  | WSPR_paBias | uint16 | Gate bias for the power amplifier stage (Q1 and Q2 on the WSPRlite board), used to adjust output power. This is a PWM duty cycle, range 0-1000. |
| 10  | WSPR_outputPower | uint8 | Nominal output power for WSPRlite in dBm. Only used by the config app, not the firmware, to allow the correct dropdown entry to be selected when loading settings. |
| 11  | WSPR_reportPower | uint8 | Transmission power reported in WSPR messages, in dBm. Note that only values ending in 0, 3, 7 are allowed by the WSPR decoder program, other values will be rounded up before transmission. |
| 12  | WSPR_txPct | uint8 | Transmission ratio. Percentage of timeslots during which the WSPRlite will transmit. The config app should limit this to be below some sensible number, like 50%. |
| 13  | WSPR_maxTxDuration | uint64 | Maximum time in seconds for which the WSPRlite will transmit after the button is pressed to start it. This is used to force the user to resynchronise the WSPRlite to an accurate clock at intervals, to avoid the timing drifting so far off that transmissions can no longer be decoded (as it would do if left on for many months/years). The config app should limit this to a sensible value. *Note:* this may be adjusted to a uint32 at some point in the future.| 
