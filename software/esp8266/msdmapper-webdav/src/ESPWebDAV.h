#include <ESP8266WiFi.h>
#include <SdFat.h>

#define DBG_INIT(...)		{ Serial.begin(__VA_ARGS__); }
#define DBG_PRINT(...) 		{ Serial.print(__VA_ARGS__); }
#define DBG_PRINTLN(...) 	{ Serial.println(__VA_ARGS__); }
// production
// #define DBG_PRINT(...) 		{ }
// #define DBG_PRINTLN(...) 	{ }

#define LED_ON				{digitalWrite(2, LOW);}
#define LED_OFF				{digitalWrite(2, HIGH);}

#define HOSTNAME_LEN 32
#define WIFI_SSID_LEN 32
#define WIFI_PASSWD_LEN 64

#define EEPROM_SIZE 512

typedef struct config_type {
  unsigned char hostname_flag;
  char hostname[HOSTNAME_LEN];
  unsigned char ssid_flag;
  char ssid[WIFI_SSID_LEN];
  unsigned char pwd_flag;
  char pwd[WIFI_PASSWD_LEN];
} CONFIG_TYPE;

// constants for WebServer
#define CONTENT_LENGTH_UNKNOWN ((size_t) -1)
#define CONTENT_LENGTH_NOT_SET ((size_t) -2)
#define HTTP_MAX_POST_WAIT 		5000 

enum ResourceType { RESOURCE_NONE, RESOURCE_FILE, RESOURCE_DIR };
enum DepthType { DEPTH_NONE, DEPTH_CHILD, DEPTH_ALL };

extern bool reboot;
extern CONFIG_TYPE config;

#ifndef FUNCTIONS_H_INCLUDED
  #define FUNCTIONS_H_INCLUDED
  void blink();
  void errorBlink();

/** year part of FAT directory date field */
static inline uint16_t FAT_YEAR(uint16_t fatDate) {
  return 1980 + (fatDate >> 9);
}
/** month part of FAT directory date field */
static inline uint8_t FAT_MONTH(uint16_t fatDate) {
  return (fatDate >> 5) & 0XF;
}
/** day part of FAT directory date field */
static inline uint8_t FAT_DAY(uint16_t fatDate) {
  return fatDate & 0X1F;
}
/** hour part of FAT directory time field */
static inline uint8_t FAT_HOUR(uint16_t fatTime) {
  return fatTime >> 11;
}
/** minute part of FAT directory time field */
static inline uint8_t FAT_MINUTE(uint16_t fatTime) {
  return(fatTime >> 5) & 0X3F;
}
/** second part of FAT directory time field */
static inline uint8_t FAT_SECOND(uint16_t fatTime) {
  return 2*(fatTime & 0X1F);
}

#endif

class ESPWebDAV	{
public:
	bool init(int chipSelectPin, int serverPort);
	bool isClientWaiting();
	void handleClient(String blank = "");
	void rejectClient(String rejectMessage);
	
protected:
	typedef void (ESPWebDAV::*THandlerFunction)(String);
	
	void processClient(THandlerFunction handler, String message);
	void handleNotFound();
	void handleReject(String rejectMessage);
	void handleRequest(String blank);
	void handleOptions(ResourceType resource);
	void handleLock(ResourceType resource);
	void handleUnlock(ResourceType resource);
	void handlePropPatch(ResourceType resource);
	void handleProp(ResourceType resource);
	void sendPropResponse(boolean recursing, File32 *curFile);
	void handleGet(ResourceType resource, bool isGet);
	void handlePut(ResourceType resource);
	void handleWriteError(String message, File32 *wFile);
	void handleDirectoryCreate(ResourceType resource);
	void handleMove(ResourceType resource);
	void handleDelete(ResourceType resource);

	// Sections are copied from ESP8266Webserver
	String getMimeType(String path);
	String urlDecode(const String& text);
	String urlToUri(String url);
	bool parseRequest();
	void sendHeader(const String& name, const String& value, bool first = false);
	void send(String code, const char* content_type, const String& content);
	void _prepareHeader(String& response, String code, const char* content_type, size_t contentLength);
	void sendContent(const String& content);
	void sendContent_P(PGM_P content);
	void setContentLength(size_t len);
	size_t readBytesWithTimeout(uint8_t *buf, size_t bufSize);
	size_t readBytesWithTimeout(uint8_t *buf, size_t bufSize, size_t numToRead);

	// variables pertaining to current most HTTP request being serviced
	WiFiServer *server;
	SdFat sd;

	WiFiClient 	client;
	String 		method;
	String 		uri;
	String 		contentLengthHeader;
	String 		depthHeader;
	String 		hostHeader;
	String		destinationHeader;

	String 		_responseHeaders;
	bool		_chunked;
	int			_contentLength;
};






