# WebDAV Server for the MSX MicroSD Mapper
This project based on the work done by ardyesp (https://github.com/ardyesp/ESPWebDAV) and synman (https://github.com/synman/ESPWebSvr/tree/sdFat-2.2.0)

The ESP8266/ESP-12E microcontroller is used in the microSD mapper project to provide convenient access to the microSD MSX card from a WebDav client, such as Windows Explorer. The microcontroller executes a program that implements a basic WebDav server, enabling access to the microSD content and allowing for wireless updates to its content.

Additionally, the microcontroller drives a small 128x32 OLED panel (optional), displaying information on the status of the WebDav server and the IP address obtained by the Wireless interface.

The wireless network can be configured through a small website hosted on the root of the server. Upon the first boot, the ESP8266 activates a SoftAP that listens for HTTP connections made to 192.168.4.1. By accessing this website through a browser, the SSID and password for the wireless connection can be configured. Once saved, this configuration is stored on the ESP8266's EPROM and reused for subsequent connections. Modifications to the wireless configuration parameters can be made at any time by returning to the website.

When the webdav server starts, the code saves a file named IP.CFG on the root of the microSD card. If access to the OLED screen is not available, the IP.CFG file can be used to determine the IP address in use.

The code also monitors the CS signal and attaches itself to the "sdmapper bus". That is to avoid any conflicts with the MSX computer trying to save files at the same time you perform the same via the WebDav server.

WiFi WebDAV server using ESP8266 SoC with changes to support the MSX MicroSD Mapper. It maintains the filesystem on an microSD card.

Supports the basic WebDav operations - *PROPFIND*, *GET*, *PUT*, *DELETE*, *MKCOL*, *MOVE* etc.

Once the WebDAV server is running on the ESP8266, a WebDAV client like Windows can access the filesystem on the SD card just like a cloud drive. The drive can also be mounted like a networked drive, and allows copying/pasting/deleting files on SD card remotely.

## Dependencies:
1. Adafruit GFX Library @ 1.11.5
2. Adafruit SSD1306 @ 2.5.7
3. SdFat @ 2.2.2

## Build And Deploy:

The card should be formatted for Fat16. The code also supports FAT32, but it works better with MSX if the card is formatted with FAT16.

This project is fully compatible with PlatformIO. Additionally, OTA is fully supported for subsequent flashes of the sketch over Wi-Fi.

Be sure to reset your adapter using its reset button after uploading a new firmware over USB.  The ESP8266 series modules will not properly reset / reboot
after a USB flash if not reset via their reset button first.  This limitation does not apply to OTA updates.

## Initial Setup
After booting the MSX with the microSD cartridge, wait approximately 60 seconds so the SoftAP gets activated. Once it indicates the WebDav server is running, connect to its Wi-Fi AP (MSXSDMAPPER).

Open up a browser and visit http://192.168.4.1

Fill in the hostname, ssid, and password fields with values that meet your needs and click the `Save` button.  Press the back button in your 
browser, refresh the page, and verify the values you entered meet your requirements.  Click the `Reboot` button to activate the changes.

After aproximiately 30 seconds, the WebDav server will be active and you can access it as described below, using the hostname you provided 
if your DHCP / DNS server support it.  Otherwise, the adapter will be available via its mDNS name `{hostname}`.`local`

You can revist the settings page at any time to reboot the adapter or update its settings.

## Usage:
To access the drive from Windows, type ```\\hostname_or_ip\DavWWWRoot``` at the Run prompt, or use Map Network Drive menu in Windows Explorer.

You can check the IP address on the IP.CFG file on the root of the microSDMapper filesystem or on the OLED Display (if available)

## Updating the ESP8266 firmware

You can use OTA to update the firmware. Load the project using PlatformIO and upload it using OTA. 




