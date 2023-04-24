# WebDAV Server and a 3D Printer
This project based on the work done by ardyesp:  https://github.com/ardyesp/ESPWebDAV

WiFi WebDAV server using ESP8266 SoC with changes to support the MSX MicroSD Mapper. It maintains the filesystem on an microSD card.

Supports the basic WebDav operations - *PROPFIND*, *GET*, *PUT*, *DELETE*, *MKCOL*, *MOVE* etc.

Once the WebDAV server is running on the ESP8266, a WebDAV client like Windows can access the filesystem on the SD card just like a cloud drive. The drive can also be mounted like a networked drive, and allows copying/pasting/deleting files on SD card remotely.

## Dependencies:
1. Adafruit GFX Library @ 1.11.5
2. Adafruit SSD1306 @ 2.5.7
3. SdFat @ 2.2.0 

## Build And Deploy:

The card should be formatted for Fat16 or Fat32

Compile and upload the program to an ESP8266 module. 
```
Detecting chip type... ESP8266
Chip is ESP8285H16
Features: WiFi, Embedded Flash
Crystal is 26MHz
MAC: xxxx
Uploading stub...
Running stub...
Stub running...
Manufacturer: a1
Device: 4015
Detected flash size: 2MB
```

This project is fully compatible with PlatformIO. Additionally, OTA is fully supported for subsequent flashes of the sketch over Wi-Fi.

Be sure to reset your adapter using its reset button after uploading a new firmware over USB.  The ESP8266 series modules will not properly reset / reboot
after a USB flash if not reset via their reset button first.  This limitation does not apply to OTA updates.

## Initial Setup
After uploading the sketch to your WiFi-SD board, reset the adapter and wait approximately 60 seconds.  You can monitor its progress via the 
PlatformIO Serial Monitor.  Once it indicates the WebDav server is running, connect to its Wi-Fi AP (msxsdmapper).

Open up a browser and visit http://192.168.4.1

Fill in the hostname, ssid, and password fields with values that meet your needs and click the `Save` button.  Press the back button in your 
browser, refresh the page, and verify the values you entered meet your requirements.  Click the `Reboot` button to activate the changes.

After aproximiately 30 seconds, the WebDav server will be active and you can access it as described below, using the hostname you provided 
if your DHCP / DNS server support it.  Otherwise, the adapter will be available via its mDNS name `{hostname}`.`local`

You can revist the settings page at any time to reboot the adapter or update its settings.

## Usage:
To access the drive from Windows, type ```\\esp_hostname_or_ip\DavWWWRoot``` at the Run prompt, or use Map Network Drive menu in Windows Explorer.




