# WSPRliteConfig

A program to adjust the settings of a WSPRlite device ( http://www.sotabeams.co.uk/wsprlite ).

License: GPLv3+

## Running

Instructions and compiled executables can be found at http://dxplorer.net/wsprlite/

### Windows

You may need to install the "CP210x USB to UART Bridge VCP" driver manually. [Download drivers here](http://www.silabs.com/products/mcu/pages/usbtouartbridgevcpdrivers.aspx).

The config program as published on dxplorer.net is statically linked, so should not need any extra DLLs.

### Linux

If the config program runs but cannot communicate with the WSPRlite, you may need to add yourself to a specific group (e.g. dialout or uucp) so that you can access the USB device. See doc/linux-serial.md for details.

## Building

First, obtain the source code:

    git clone https://github.com/SOTAbeams/WSPRliteConfig.git
    cd WSPRliteConfig
    git submodule update --init


### Dependencies

```
cmake
g++
git-core
wxWidgets (package name is usually something like libwxgtk3.0-dev)
libserialport (v0.1.1 or later)
```

Note that the current Debian stable version of libserialport is too old (v0.1.0), so on some distributions you may need to compile it from source code.

### Linux

    mkdir build
    cd build
    cmake ..
    make

Remember to run `git submodule update --init` first.
 
### Windows

This program uses some modern C++ features. This means it might or might not successfully compile in Visual Studio.


### Libraries used

* [libserialport](http://sigrok.org/wiki/Libserialport) - GNU LGPLv3+
* wxWidgets - wxWindows Library Licence
* [CRC++](https://github.com/d-bahr/CRCpp) - 3-clause BSD license
* [cppcodec](https://github.com/tplgy/cppcodec) - MIT/Expat license

CRC++ and cppcodec are header only libraries and are set up as git submodules in this repository, so should not need to be installed in order to compile the config program. libserialport and wxWidgets (and their development packages if applicable) need to be installed to compile the program.