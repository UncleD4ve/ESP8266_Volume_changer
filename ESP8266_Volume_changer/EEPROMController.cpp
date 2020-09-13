#include "EEPROMController.h"

EEPROMController::EEPROMController()
{
	EEPROM.begin(512);
	delay(10);
}

bool EEPROMController::saveWifi(String & ssid, String & password)
{
	for (int i = 64; i < 192; ++i)
		EEPROM.write(i, 0);

	EEPROM.end();
	EEPROM.begin(512);
	delay(10);

	if (ssid.length() > 64 || password.length() > 64)
	{
		Serial.print(F("eeprom overflow"));
		return false;
	}

	for (int i = 0; i < ssid.length(); ++i)
		EEPROM.write(64 + i, ssid[i]);

	for (int i = 0; i < password.length(); ++i) 
		EEPROM.write(64 + 64 + i, password[i]);

	bool state = EEPROM.commit();
	Serial.print(F("Save to eeprom: "));
	Serial.println(state);
	return state;
}

void EEPROMController::readWifi(String & ssid, String & password)
{
	char ssidArr[64];
	char passArr[64];

	for (int i = 0; i < 64; ++i)
		ssidArr[i] = char(EEPROM.read(i + 64));

	for (int i = 0; i < 64; ++i)
		passArr[i] = char(EEPROM.read(i + 64 + 64));

	ssid = String(ssidArr);
	password = String(passArr);
}

void EEPROMController::displayWiFi()
{
	char ssidArr[64];
	char passArr[64];

	for (int i = 0; i < 64; ++i)
		ssidArr[i] = char(EEPROM.read(i + 64));

	for (int i = 0; i < 64; ++i)
		passArr[i] = char(EEPROM.read(i + 64 + 64));

	Serial.print(F("Wifi ssid: "));
	Serial.println(String(ssidArr));
	Serial.print(F("Wifi pass: "));
	Serial.println(String(passArr));
}

bool EEPROMController::setConfig(bool config)
{
	EEPROM[0] = config;
	EEPROM[1] = (char)90;
	EEPROM[2] = (char)90;
	return EEPROM.commit();
}

void EEPROMController::resetConfig()
{
	bool response = setConfig(false);
	Serial.print(F("EEPROM: "));
	Serial.println(response);
	WiFi.disconnect(true);
	delay(100);
	ESP.restart();
}

bool EEPROMController::getConfig()
{
	return EEPROM[0];
}


