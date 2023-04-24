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

The ESP8266/ESP-12E microcontroller is used in the microSD mapper project to provide convenient access to the microSD card from a WebDav client, such as Windows Explorer. The microcontroller executes a program that implements a basic WebDav server, enabling access to the microSD content and allowing for wireless updates to its content.

Additionally, the microcontroller drives a small 128x32 OLED panel, displaying information on the status of the WebDav server and the IP address obtained by the Wireless interface.

The wireless network can be configured through a small website hosted on the root of the server. Upon the first boot, the ESP8266 activates a SoftAP that listens for HTTP connections made to 192.168.4.1. By accessing this website through a browser, the SSID and password for the wireless connection can be configured. Once saved, this configuration is stored on the ESP8266's EPROM and reused for subsequent connections. Modifications to the wireless configuration parameters can be made at any time by returning to the website.

When the webdav server starts, the code saves a file named IP.CFG on the root of the microSD card. If access to the OLED screen is not available, the IP.CFG file can be used to determine the IP address in use.

## BIOS

[TBI]

## License 

This work is licensed under the CERN OHL-S v2. You may redistribute and modify this project and its documentation under the terms of the CERN-OHL-S v2.

![Open Hardware](images/1024px-Open-source-hardware-logo.svg.png)