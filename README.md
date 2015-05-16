# About

This library provides support for Pulse-Eight's USB-CEC adapter and other CEC capable hardware, like the Raspberry Pi.

You can find a list of frequently asked questions on [libCEC's FAQ page] (http://libcec.pulse-eight.com/faq)

# Supported platforms

## Linux & BSD

libCEC needs the following dependencies in order to work correctly:
* [libplatform] (https://github.com/Pulse-Eight/platform) 1.0 or later
* udev v151 or later
* cdc-acm support compiled into the kernel or available as module
* [liblockdev] (https://packages.debian.org/search?keywords=liblockdev) 1.0 or later

To compile libCEC on Linux, you'll need the following dependencies:
* [cmake 2.6 or better] (http://www.cmake.org/)
* a supported C++ 11 compiler
* [liblockdev] (https://packages.debian.org/search?keywords=liblockdev) 1.0 development headers

The following dependencies are recommended. Without them, the adapter can not
be (fully) auto-detected.
* pkg-config
* udev development headers v151 or later
* X randr development headers

To compile, execute the following commands:
```
mkdir build
cd build
cmake ..
make
sudo make install
```

## Apple OS X

To compile libCEC on OS X, you'll need the following dependencies:
* [libplatform] (https://github.com/Pulse-Eight/platform) 1.0 or later
* [cmake 2.6 or better] (http://www.cmake.org/)
* a supported C++ 11 compiler
* xcode 3.2.6 or later

To compile, execute the following command:
```
mkdir build
cd build
cmake ..
make
sudo make install
```

_Note:_ You may need to copy pkg.m4 to your m4 sources directory

## Microsoft Windows

To compile libCEC on Windows, you'll need the following dependencies:
* [libplatform] (https://github.com/Pulse-Eight/platform) 1.0 or later
* [cmake 2.6 or better] (http://www.cmake.org/)
* [Visual Studio 2013] (https://www.visualstudio.com/)
* [Windows DDK (Driver Development Kit)] (https://msdn.microsoft.com/en-us/windows/hardware/hh852365.aspx)
* To create an installer, you'll need [Nullsoft's NSIS] (http://nsis.sourceforge.net/)

When compiling LibCecSharp, you'll need the following versions too:
* Visual Studio 2012
* Visual Studio 2010
* Visual Studio 2008

To compile libCEC, follow these instructions:
* run support/build.cmd to build libCEC and cec-client
* open /project/libcec.sln with Visual Studio 2013.
* build the project.

To develop for libCEC or cec-client in Visual Studio:
* run support/visual-studio.cmd

To build an installer on Windows:
* go to /project and execute create-installer.bat to create the installer.
* the installer is stored as /build/libCEC-installer.exe

# Developers

We provide a C, C++, Python and .NET CLR interface to the adapter.

## C++ developers
* the API can be found in /include/cec.h
* an example implementation can be found in /src/cec-client/cec-client.cpp

## C developers
* the API can be found in /include/cecc.h
* an example implementation can be found in /src/cecc-client/cecc-client.cpp

## .NET developers
* add a reference to LibCecSharp.dll
* an example can be found in \src\CecSharpTester\CecSharpClient.cs

## Python developers
* the API is exported to Python through Swig
* an example can be found in \src\pyCecClient

# Developers Agreement

If you wish to contribute to this project, you must first sign our contributors agreement.
Please see [the contributors agreement] (http://www.pulse-eight.net/contributors) for more information.
