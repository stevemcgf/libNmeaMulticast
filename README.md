# libNmeaMulticast

This library is a C++ implementation of a NMEA Ethernet interconnection as defined in EN 61162-450:2011, "Maritime navigation and radiocommunication equipment and systems - Digital interfaces - Part 450: Multiple talkers and multiple listeners - Ethernet interconnection."

## Description

This library allows to communicate using multicast. Have support for talking and listening to NMEA ethernet devices.

## Motivation

I didn't find any available implementation of this standard. This library is part of a larger project currently in use on many ships.

## Installation

If you use CMake you can simple add this directory to your project and refer to it using **target_link_libraries**. You can also compile then copy the static library and include directory.

## API Reference

The code has doxygen documentation can be generated using "make doc.NmeaMulticast"

## License

GNU LESSER GENERAL PUBLIC LICENSE Version 3, 29 June 2007
