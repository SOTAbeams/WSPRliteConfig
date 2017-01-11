# Physical connection

## USB

Communication between the config program and the WSPRlite is by means of a USB-to-UART serial bridge (currently a Silicon Labs CP2104 chip).

The USB device description is set during manufacture to "SOTAbeams WSPRlite".

## Serial connection

Serial settings are:

* Baud rate: 1Mbps
* 8 data bits, 2 stop bits, no parity
* Flow control: RTS/CTS

Note: flow control is not fully supported.

RTS (pause flow from WSPRlite to computer) is deliberately not implemented in the hardware, since the USB-to-UART chip has quite a large buffer (576 bytes), and the WSPRlite only ever sends data back to the computer in response to messages from the computer. To pause flow from the WSPRlite to computer, simply stop sending data from the computer to the WSPRlite.

CTS (pause flow from computer to WSPRlite) does not appear to work correctly, at least on Linux. This might be a bug in the libserialport library. Workaround: only send one message at a time to the WSPRlite, wait for the response before sending the next message.

Testpoints TP1 and TP2 on the WSPRlite board are connected to the RX and TX pins for the serial connection.

# Message format

Integers are little-endian.

The bytes which are sent through the serial connection for a single message are:

    transmittedBytes ::= start escapedMessage end
    escapedMessage ::= (plainByte | escapeSeq)*

    controlByte ::= start | end | esc
    plainByte ::= (uint8 - controlByte)   ; Any byte except one of the controlBytes
    escapeSeq ::= esc escapedByte

    start ::= '\x01'
    end ::= '\x04'
    esc ::= '\x10'
    ; The escaped version of each controlByte is obtained by adding 0x80 to the controlByte
    escapedByte ::= '\x81' | '\x84' | '\x90' 

After unescaping `escapedMessage`:

    message ::= msgType msgData checksum
    msgType ::= uint16
    msgData ::= (uint8)*
    checksum ::= uint32

The checksum is the CRC32 of `msgData`.

A list of `msgType` values can be found in src/common/device/MsgType.hpp



# Messages from WSPRlite to computer

The WSPRlite only sends messages in response to commands sent by the computer. Note that there is no tracking built into the protocol of which command a message is replying to, though the WSPRlite is guaranteed to process and respond to messages sequentially. Unless you have a good reason to do otherwise, send one message at a time and wait for a response before sending the next one, to avoid losing track of which response was for which command.

### ACK

Indicates that the command was successful. No msgData.

### NACK

Indicates that the command was not successful.

msgData may be present. If it is, it will be a null-terminated string which is the error message.

### ResponseData

Indicates that the command was successful and has returned some data. The meaning of msgData depends on what the original command was - see "from computer to WSPRlite" section below.

# Messages from computer to WSPRlite

### Version

Retrieves information about firmware and hardware version. 

No command data.

Reply: ResponseData

    msgData ::= deviceVersion firmwareVersion
    deviceVersion ::= productId productRevision bootloaderVersion
    firmwareVersion ::=  major minor patch date

All numbers (productId, productRevision, bootloaderVersion, major, minor, patch, date) are uint32.

deviceVersion is currently 1,1,1 for the WSPRlite.

### Read

Read a config variable.

Command data:

    msgData ::= variableId
    variableId ::= uint16

Reply: NACK or ResponseData. Contents of ResponseData will depend on which variable is being read - see cfgvars.md for details.

### Write

Write a config variable.

Command data:

    msgData ::= variableId variableData
    variableId ::= uint16

variableData will depend on which variable is being written - see cfgvars.md for details.

Reply: ACK or NACK.

### Reset

Reboots the device. No command data. Reply: ACK.

### DeviceMode_Get

Gets some information about what the WSPRlite is currently doing. Supported by firmware v1.0.4 and later, limited support in earlier versions.

No command data.

Reply: ResponseData

    msgData ::= deviceMode | deviceMode deviceModeSub
    deviceMode ::= uint16
    deviceModeSub ::= uint16

See src/common/device/DeviceMode.hpp for valid device mode values, and WSPRConfigFrame::startStatusUpdate for hints on what they mean.

deviceModeSub is only present for some deviceModes (currently, only DeviceMode::WSPR_Active).

### DeviceMode_Set

Sets the current device state.

E.g. setting to DeviceMode::WSPR_Active has the same effect as pressing the button on the WSPRlite. (This is currently unimplemented in the config program, since the config program does not yet have a way of checking the accuracy of the computer time.)

Command data for most device modes is:

    msgData ::= deviceMode
    deviceMode ::= uint16

For DeviceMode::Test_ConstantTx, which temporarily makes the WSPRlite emit a constant tone for testing purposes:

    msgData ::= deviceMode frequency paBias
    frequency ::= uint64
    paBias ::= uint16

`frequency` is the output frequency in Hz. `paBias` controls the gate bias for the power amplifier stage, which affects the output power of the WSPRlite. It is a PWM duty cycle, range 0-1000.

Reply: ACK or NACK

### Bootloader_State

Checks whether the device is in bootloader (firmware update) mode.

No command data.

Reply: ResponseData.

    msgData ::= bootloaderMode
    bootloaderMode ::= '\x00' | '\x01' | '\x02'

0=in normal mode, 1=in bootloader mode, 2=in bootloader mode with no valid firmware present to reboot into.

### Bootloader_Enter
### Bootloader_EraseAll,
### Bootloader_ErasePage,
### Bootloader_ProgramHexRec,
### Bootloader_ProgramRow,
### Bootloader_ProgramWord,
### Bootloader_CRC,
### Bootloader_ProgramResetAddr

Currently undocumented since they are likely of limited interest. Note that you might break your WSPRlite if you use these incorrectly, to the extent of needing to use a PICkit or similar to fix it.

### DumpEEPROM
### WSPR_GetTime

Currently undocumented since they have not been properly tested yet, and might or might not remain in the firmware.