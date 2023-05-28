// Using the WebDAV server with MSX microSD mapper
// Author: Cristiano Goncalves
// Based on work of synman available at https://github.com/synman/ESPWebSvr/tree/sdFat-2.2.0

#include <Arduino.h> //conversion to platformio
#include <ArduinoOTA.h> //using over the air to update the firmware

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <user_interface.h>
#include <SPI.h>
#include <SdFat.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "ESPWebDAV.h"

#define OLED_DISPLAY 1 //set to 1 if you have a 128x32 oled display connected to the board

#define BM_WDT_SOFTWARE 0
#define BM_WDT_HARDWARE 1
#define BM_ESP_RESTART 2
#define BM_ESP_RESET 3

#define BOOT_MODE BM_WDT_SOFTWARE
#define HOSTNAME    "MSXSDMAPPER"

// LED is connected to GPIO2 on this board
#define INIT_LED			{pinMode(2, OUTPUT);}

#define SD_CS		4
#define MISO		12
#define MOSI		13
#define SCLK		14
#define CS_SENSE	5

//only if the board supports OLED_DISPLAY (need to be ESP12E to have enough pins)
#if OLED_DISPLAY == 1
// OLED_DISPLAY pins
#define DSP_SCA 	9
#define DSP_SCK 	10
// OLED_DISPLAY control
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin not used
#endif

#define SERVER_PORT		80
#define SPI_BLOCKOUT_PERIOD	20000UL

uint32 sys_time = system_get_time();
CONFIG_TYPE config;

const char *hostname 	= HOSTNAME;
const char *ssid 		= "";
const char *pwd 		= "";
WiFiMode_t wifimode     = WIFI_AP;

#if OLED_DISPLAY == 1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

ESPWebDAV dav;
String statusMessage;
bool initFailed = false;

volatile unsigned long spiBlockoutTime = 0;
bool weHaveBus = false;

#if OLED_DISPLAY == 1
//ghost bitmap
const unsigned char PACMAN_GHOST [] PROGMEM = {
    0b00000000, 0b00000000, // Row 1
    0b00000011, 0b11000000, // Row 2
    0b00001111, 0b11110000, // Row 3
    0b00011111, 0b11111000, // Row 4
    0b00111111, 0b11111100, // Row 5
    0b00110011, 0b11001100, // Row 6
    0b00100001, 0b10000100, // Row 7
    0b01100001, 0b10000110, // Row 8
    0b01100001, 0b10000110, // Row 9
    0b01110011, 0b11001110, // Row 10
    0b01111111, 0b11111110, // Row 11
    0b01111111, 0b11111110, // Row 12
    0b01111111, 0b11111110, // Row 13
    0b01111011, 0b11011110, // Row 14
    0b00110001, 0b10001100, // Row 15
    0b00000000, 0b00000000  // Row 16
};
#endif

// ------------------------
void takeBusControl()	{
// ------------------------
	weHaveBus = true;
	LED_ON;
	pinMode(MISO, SPECIAL);	
	pinMode(MOSI, SPECIAL);	
	pinMode(SCLK, SPECIAL);	
	pinMode(SD_CS, OUTPUT);
}

// ------------------------
void relenquishBusControl()	{
// ------------------------
	pinMode(MISO, INPUT);	
	pinMode(MOSI, INPUT);	
	pinMode(SCLK, INPUT);	
	pinMode(SD_CS, INPUT);
	LED_OFF;
	weHaveBus = false;
}

// ------------------------
void blink()	{
// ------------------------
	LED_ON; 
	delay(100); 
	LED_OFF; 
	delay(400);
}

// ------------------------
void errorBlink()	{
// ------------------------
	for(int i = 0; i < 100; i++)	{
		LED_ON; 
		delay(50); 
		LED_OFF; 
		delay(50);
	}
}

#if OLED_DISPLAY == 1
//display messages on the 128x32 oled screen
void Display(String msg1, String msg2)
{
    display.clearDisplay();
    //show the logo on the OLED_DISPLAY on the left side, vertical centralized
    int16_t y = (SCREEN_HEIGHT - 14) / 2; // calculate the vertical position
    display.drawBitmap(0, y, PACMAN_GHOST, 16, 16, WHITE);
    //set cursor
    display.setTextSize(1);
    display.setTextColor(WHITE);
    //display first line
    display.setCursor(20, 4);
    display.print("microSD Mapper 1.2");
    display.setCursor(20, 14);
    display.print(msg1);
    display.setCursor(20, 24);
    display.print(msg2);
    // Update the display
    display.display();
}
#endif

// ------------------------
void setup() {
  	DBG_INIT(115200);
	DBG_PRINTLN("\nMSXSDMAPPER setup");

#if OLED_DISPLAY == 1
	Wire.begin(DSP_SCA, DSP_SCK); // SDA, SCK
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.display();
#endif

	EEPROM.begin(EEPROM_SIZE);
	uint8_t *p = (uint8_t*)(&config);
	for (uint8 i = 0; i < sizeof(config); i++)
	{
		*(p + i) = EEPROM.read(i);
	}
	EEPROM.commit();

	if (config.hostname_flag == 9) {
		DBG_PRINT("config host="); DBG_PRINTLN(config.hostname);
		hostname = config.hostname;
	} 
	if (config.ssid_flag == 9) {
		DBG_PRINT("config ssid="); DBG_PRINTLN(config.ssid);
		ssid = config.ssid;
		wifimode = WIFI_STA;
	} 
	if (config.pwd_flag == 9) {
		DBG_PRINT("config pwd="); DBG_PRINTLN(config.pwd);
		pwd = config.pwd;
	} 

	INIT_LED;
	blink();
	
	WiFi.hostname(hostname);
	WiFi.mode(wifimode);
	WiFi.setAutoConnect(false);
	WiFi.setPhyMode(WIFI_PHY_MODE_11N);

	if (wifimode != WIFI_AP) {
		WiFi.begin(ssid, pwd);
		// Wait for connection
		DBG_PRINT("Connecting to WiFi .");
#if OLED_DISPLAY == 1
		Display("Connecting to WiFi",ssid);
#endif
		for (uint8 x = 0 ; x < 60 && WiFi.status() != WL_CONNECTED; x++) {
			blink();
			DBG_PRINT(".");
		}
		
	}
	
	if (WiFi.status() != WL_CONNECTED) {
		wifimode = WIFI_AP;
		WiFi.softAP(HOSTNAME);
		DBG_PRINTLN("\nSoftAP [MSXSDMAPPER / 192.168.4.1] started");
	}

	if (!MDNS.begin(hostname)) { 
		DBG_PRINTLN("\nError setting up mDNS responder!");
	} else {
		DBG_PRINTLN("\nmDNS successfully activated");
		MDNS.update();
	}
  
   	ArduinoOTA.begin();

	DBG_PRINTLN("");
  	DBG_PRINT("    Hostname: "); DBG_PRINTLN(hostname);
	DBG_PRINT("Connected to: "); DBG_PRINTLN(ssid);
	DBG_PRINT("  IP address: "); DBG_PRINTLN(WiFi.localIP());
	DBG_PRINT("        RSSI: "); DBG_PRINTLN(WiFi.RSSI());
	DBG_PRINT("        Mode: "); DBG_PRINTLN(WiFi.getPhyMode());

	DBG_PRINT("\nAttaching to the SD bus .");

	// Detect when other master uses SPI bus
	pinMode(CS_SENSE, INPUT);
	attachInterrupt(CS_SENSE, []() {
		if(!weHaveBus)
			spiBlockoutTime = millis() + SPI_BLOCKOUT_PERIOD;
	}, FALLING);

	// sleep for 30 seconds
	// we don't want to be touching the SD card 
	// if we are connected to a printer and its
	// bootloader is trying to install a firmware bin
	for (uint8 x = 0 ; x < 30 ; x++) {
		DBG_PRINT(".");
		blink();
		delay(500);
#if OLED_DISPLAY == 1
		Display(WiFi.localIP().toString(),"Starting...");
#endif
	}

	for (uint8 x = 0 ; x * 1000 < SPI_BLOCKOUT_PERIOD ; x = x + 1) {
		// wait for other master to assert SPI bus first
		DBG_PRINT(".");
		blink();
		delay(500);
	}
  
	// ----- SD Card and Server -------
	// Check to see if other master is using the SPI bus
	while(millis() < spiBlockoutTime) {
		DBG_PRINT(".");
		blink();
	}
	
	takeBusControl();
	
	// start the SD DAV server
	if(!dav.init(SD_CS, SERVER_PORT)) {
		statusMessage = "Failed to initialize SD Card";
		DBG_PRINT("\nERROR: "); DBG_PRINTLN(statusMessage);
#if OLED_DISPLAY == 1
		Display("ERROR",statusMessage);
#endif
		// indicate error on LED
		errorBlink();
		initFailed = true;
	} else {
		blink();
		DBG_PRINTLN("\nWebDAV server started");
#if OLED_DISPLAY == 1
		Display(WiFi.localIP().toString(),"WebDAV Ready!");
#endif
  	}

	relenquishBusControl();
}

// ------------------------
void loop() {
// ------------------------
	MDNS.update();
	ArduinoOTA.handle();

	if(millis() < spiBlockoutTime)
		blink();

	// do it only if there is a need to read FS
	if(dav.isClientWaiting())	{
		if(initFailed)
			return dav.rejectClient(statusMessage);
		
		// has other master been using the bus in last few seconds
		if(millis() < spiBlockoutTime)
			return dav.rejectClient("MSX is reading from SD card. Wait a few seconds and refresh.");
		
		// a client is waiting and FS is ready and other SPI master is not using the bus
		takeBusControl();
		dav.handleClient();
		relenquishBusControl();
	}

	// reboot every 6 hours
	if (sys_time / 1000 / 1000 + 21600 <= system_get_time() / 1000 / 1000)  {
		DBG_PRINT("Requesting scheduled reboot ref="); DBG_PRINT(sys_time / 1000 / 1000 + 21600); DBG_PRINT(" cur="); DBG_PRINTLN(system_get_time() / 1000 / 1000);
		reboot = true;    
	}

	if (reboot || initFailed) {
		initFailed = false;
		reboot = false;
		DBG_PRINTLN("REBOOTING");
		errorBlink();
		#if BOOT_MODE == BM_ESP_RESTART
		Serial.println("\n\nRebooting with ESP.restart()");
		ESP.restart();
		#elif BOOT_MODE == BM_ESP_RESET
		Serial.println("\n\nRebooting with ESP.reset()");
		ESP.reset();
		#else 
		#if BOOT_MODE == BM_WDT_HARDWARE      
			Serial.println("\n\nRebooting with hardware watchdog timeout reset");
			ESP.wdtDisable();
		#else
			Serial.println("\n\nRebooting with sofware watchdog timeout reset");
		#endif  
		while (1) {}  // timeout!
		#endif 
	}
}

