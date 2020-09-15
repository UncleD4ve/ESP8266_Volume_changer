#ifndef WiFiRegister_h
#define WiFiRegister_h
#include "Arduino.h"
#include "indexBegin.h"
#include "indexEnd.h"
#include "WiFiRegister.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "EEPROMController.h"

class WiFiRegister {
public:
	WiFiRegister(const char*);
	WiFiRegister();
	void begin();
private:
	ESP8266WebServer _server;
	DNSServer dnsServer;
	WiFiClient _client;
	String _ssid;
	String _pass;
	const char * _apName;
	char _status[13];

	uint8_t encryptionTypeStr(uint8_t);
	uint8_t encryptionPowerStr(int8_t);
	const  char * encryptionColorStr(int8_t);
	String constructHTMLpage();

	void ssidFromWeb();
	void handleRoot();
	void status();
	void restart();
};
#endif