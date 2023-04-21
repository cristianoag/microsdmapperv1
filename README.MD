# MSX microSD Mapper
The MSX microSD Mapper is a project aimed at building upon the success of Luciano Sturaro and Fabio Belavenuto's implementation of the well known MSX SD Mapper.

It features a single slot for microSD cards, a ESP8266 (ESP12E) micro-controller to offer wireless access, and simplified board design. 

With microSD cards becoming increasingly affordable and widely available, that seems to be a better option now. Additionally, the original SD Mapper v2 project had some confusion around the switches used to control the megaram/mapper, which were later changed in versions of the firmware to enable and disable the mapper on v2.

The ESP8266 enables network connectivity for the microSD card when it is inserted into the cartridge slot. Through a WebDAV server, it provides a convenient method for accessing and modifying the cartridge storage from any networked PC at any time, without requiring adapters, and eliminating the need for repeatedly removing and reinserting the memory card.

## Hardware Versions

* [microSDmapper v1.0](hardware/microsdmapper_v1.0/) - Initial test board with the basic configuration. 
* [microSDmapper v1.1](hardware/microsdmapper_v1.1/) - Prototype board with the initial circuit, basic wiring for the ESP12E/8266 and a pin header to wire a display to show the MAC, and IP address acquired via DHCP. (In development)

## CPLD Firmware

[TBI]

## ESP8266/ESP12E Firmware

[TBI]

## BIOS

[TBI]

## License 

This work is licensed under the CERN OHL-S v2. You may redistribute and modify this project and its documentation under the terms of the CERN-OHL-S v2.

![Open Hardware](images/1024px-Open-source-hardware-logo.svg.png)