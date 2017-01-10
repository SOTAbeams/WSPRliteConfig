### Running the config program on Linux

You may need to change some permissions in order for the config program to be able to open the serial port to communicate with the WSPRlite.

If the config program is unable to talk to the WSPRlite, first find out the USB device name. From a terminal, run `ls /dev/ttyUSB*` before and after plugging in your WSPRlite. The new device will be your WSPRlite. For example, `/dev/ttyUSB0`.

Next, check whether you can write to the device:

    touch /dev/ttyUSB0

If this works fine, you have an unknown problem. Try looking at the last few lines of `dmesg`.

If this comes up with an error, check the permissions of the device and which groups you belong to:

    ls -l /dev/ttyUSB0
    groups

Example result (Debian):

```
$ ls -l /dev/ttyUSB0
crw-rw-rw- 1 root dialout 188, 0 Jan 1 12:34 /dev/ttyUSB0
$ groups
johnsmith dialout cdrom floppy audio dip video plugdev netdev lpadmin scanner bluetooth
```

This should work fine, you shouldn't need to be in a specific user group to access the device.

Or on a different computer (Gentoo):

```
$ ls -l /dev/ttyUSB0
crw-rw---- 1 root uucp 188, 0 Jan 1 12:34 /dev/ttyUSB0
$ groups
wheel floppy audio cdrom video games usb users plugdev portage johnsmith
```

The permissions are `rw-rw----`, which mean you need to be in the group that owns the USB device (in this case, `uucp` which is not listed as one of your groups) in order to use it.

Assuming your username is johnsmith and the group that owns the USB device is uucp, run:

    sudo usermod -a -G uucp johnsmith

You will then need to log out then back in again, or restart your computer.
