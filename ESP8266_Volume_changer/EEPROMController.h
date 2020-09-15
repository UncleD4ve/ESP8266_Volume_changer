#ifndef EEPROMController_h
#define EEPROMController_h
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>

class EEPROMController {
public:
	EEPROMController();
	bool saveWifi(String &, String &);
	void readWifi(String &, String &);
	void displayWiFi();
	bool setConfig(bool);
	void resetConfig();
	bool getConfig();
	bool isStaticAddres();
	bool setStaticAddres(bool);
	bool setIpConfig();
	IPAddress getIp();
	IPAddress getGateway();
	IPAddress getSubnet();
	bool setIp(IPAddress&);

	template<typename T>
	bool setVar(int const address, const T &t) {
		EEPROM.put(address, t);
		return EEPROM.commit();
	};

	template<typename T>
	T &getVar(int const address, T &t) {
		return EEPROM.get(address, t);
	};

};
#endif