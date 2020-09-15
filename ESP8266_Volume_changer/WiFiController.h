#ifndef WiFiController_h
#define WiFiController_h
#include "WiFiRegister.h"
#include "EEPROMController.h"
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>

class WiFiController {
public:
	WiFiController();
	bool begin(const char* SSID, int8_t mode = 0, bool wake = true);
	void forceWifiERegister();
	bool connect();
	bool checkInternet();
	bool changeMode(int8_t mode = 4);
	void dnsLoop();
	void resetESP();

private:
	EEPROMController eeprom;
	WiFiClient _client;
	DNSServer dnsServer;

	String _ssid;
	String _pass;
	const char* _apName;
	int8_t _mode;

	bool modeSTA();
	bool modeAP();
};
#endif