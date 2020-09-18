#ifndef WebServerController_h
#define WebServerController_h
#include "Arduino.h"
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include "WiFiController.h"
#include <DNSServer.h>

class WebServerController {
public:
	WebServerController(uint8_t&, uint8_t&);
	void beginOTA(const char*, const char*);
	void beginSPIFFS();
	void beginWebSocket();
	void beginServer();

	void webSocketSend(char sign, uint8_t num);

	void webSocketLoop();
	void handleClientLoop();
	void otaLoop();

	WiFiController WiFiContr;
private:
	ESP8266WebServer _server;
	WebSocketsServer _webSocket;
	File _fsUploadFile;
	EEPROMController eeprom;

	void handleRoot();
	void handleFileUpload();
	bool handleFileRead(String);

	void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
	String formatBytes(size_t);		//simplify
	String getContentType(String);	//simplify

	uint8_t & _position;
	uint8_t & _destPosition;
	char buffer[5];
};
#endif