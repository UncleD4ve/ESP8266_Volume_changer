#include "WiFiRegister.h"

WiFiRegister::WiFiRegister(const char * ssid) :_server(80), _apName(ssid) {}
WiFiRegister::WiFiRegister() : _server(80) {}

void WiFiRegister::begin()
{
	Serial.println(ESP.getFreeHeap(), DEC);
	WiFi.forceSleepWake();
	delay(500);

	IPAddress apIP(5, 5, 5, 5);

	WiFi.disconnect(true);
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

	dnsServer.setTTL(1);
	dnsServer.start(53, "*", apIP);

	if (WiFi.softAP(strlen(_apName) == 0 ? String(F("ESP WiFi Register")) : _apName)) {
		Serial.print(F("\nWifi Register\nAccess Point name: "));
		Serial.print(WiFi.softAPSSID());
		Serial.print(F("\nIP addres:"));
		Serial.println(WiFi.softAPIP().toString().c_str());
	}
	else {
		Serial.println(F("FAILED to create Access Point, restart!"));
		yield();
		ESP.restart();
	}

	WiFi.scanNetworks(false, false);
	_server.on(F("/"),			std::bind(&WiFiRegister::handleRoot, this));
	_server.on(F("/connect"),	std::bind(&WiFiRegister::ssidFromWeb, this));
	_server.on(F("/status"),	std::bind(&WiFiRegister::status, this));
	_server.on(F("/restart"),	std::bind(&WiFiRegister::restart, this));
	_server.begin();

	while (true)
	{
		_server.handleClient();
		dnsServer.processNextRequest();
		yield();
	}
}

String WiFiRegister::constructHTMLpage() {

	int numSSID(WiFi.scanNetworks(false, false));
	int indices[numSSID];

	for (int i = 0; i < numSSID; i++)
		indices[i] = i;

	for (int i = 0; i < numSSID; i++)
		for (int j = i + 1; j < numSSID; j++)
			if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
				std::swap(indices[i], indices[j]);

	String HTMLpage(FPSTR(indexBegin));
	if (numSSID > 0)
	{
		HTMLpage += F("<h4 id='avNetworks'>Available networks</h4><br>");
		for (int i = 0; i < numSSID; i++) {
			HTMLpage.concat(F("<div class = \"button\" onclick = \"getSSID('"));
			HTMLpage.concat(WiFi.SSID(indices[i]));
			HTMLpage.concat(F("')\" style = \"background-image: linear-gradient(-225deg, #a8edea 60%, "));
			HTMLpage.concat(encryptionColorStr(WiFi.RSSI(indices[i])));
			HTMLpage.concat(F(" 100% ); \"> <div class = \"buttonMarg\"><div class = \"left\">"));
			HTMLpage.concat(WiFi.SSID(indices[i]));
			HTMLpage.concat(F("</div><div class = \"center\">&#12827"));
			HTMLpage.concat(encryptionTypeStr(WiFi.encryptionType(indices[i])));
			HTMLpage.concat(F("</div><div class = \"right\">"));
			HTMLpage.concat(encryptionPowerStr(WiFi.RSSI(indices[i])));
			HTMLpage.concat(F("%</div></div></div><br>"));
		}
	}
	else
		HTMLpage.concat(F("<h4 id='avNetworks'>No networks available!</h4><br>"));
	HTMLpage.concat(F("<h4>SSID</h4><div class=\"alpha\"><input type=\"text\" id=\"SSID\" name=\"SSID\" placeholder=\"SSID..\"></div><h4>Password</h4><div class=\"alpha\"><input type=\"Password\" id=\"Password\" name=\"Password\" placeholder=\"Pass..\"></div></p><input type=\"button\" value=\"Connect\" onclick=\"connect()\"><input type=\"button\" value=\"Reload\" onclick=\"reload()\"><h4>MAC Address:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"));
	HTMLpage.concat(WiFi.macAddress());
	HTMLpage.concat(F("</h4>"));
	HTMLpage.concat(FPSTR(indexEnd));
	return HTMLpage;
}

void WiFiRegister::handleRoot()
{
	Serial.println(ESP.getFreeHeap(), DEC);
	_server.send(200, F("text/html"), constructHTMLpage());
}

void WiFiRegister::status()
{
	_server.send(200, F("text/html"), _status);
}

void WiFiRegister::restart()
{
	if (_status[0] == 'I')
	{
		EEPROMController eeprom;
		if (eeprom.saveWifi(_ssid, _pass))
			if (eeprom.setConfig(true)) {
				yield();
				eeprom.setIpConfig();
				yield();
				Serial.println(F("Device is restarting!"));
				WiFi.softAPdisconnect();
				yield(); 
				WiFi.disconnect(true);
				delay(3000);
				ESP.restart();
			}
			else
				Serial.println(F("eeprom problem!"));
	}
	_server.send(200, F("text/html"), F("Server encountered an error"));
}

void WiFiRegister::ssidFromWeb() {
	if (_server.arg(F("key")) == F("19") && _status[0] != 'W') {
		_server.send(200, F("text/html"), F("ok"));
		yield();

		strcpy_P(_status, PSTR("W1"));
		Serial.println(F("Connecting"));

		_ssid = _server.arg(F("ssid"));
		_pass = _server.arg(F("pass"));

		uint8_t i(0), multi(4);
		unsigned long previousMillis = 0;

		WiFi.begin(_ssid, _pass);
		while (WiFi.status() != WL_CONNECTED && i <= 10 * multi) {

			unsigned long currentMillis = millis();
			if (currentMillis - previousMillis >= 500) {
				previousMillis = currentMillis;
				if (++i % multi == 0)
				{
					strcpy_P(_status, PSTR("W"));
					strcat(_status, String(i / multi).c_str());
					Serial.println(i / multi);
				}
			}
			_server.handleClient();
			dnsServer.processNextRequest();
			yield();
		}
		yield();
		if (WiFi.status() == WL_CONNECTED)
		{
			strcpy_P(_status, PSTR("I"));
			strcat(_status, WiFi.localIP().toString().c_str());
		}
		else
		{
			strcpy_P(_status, PSTR("N"));
			WiFi.scanNetworks(false, false);
		}
	}
	else
	_server.send(200, F("text/html"), F("nok"));
}

uint8_t WiFiRegister::encryptionTypeStr(uint8_t authmode) {
	switch (authmode) {
	case ENC_TYPE_NONE:
		return 5;
	default:
		return 4;
	}
}

uint8_t WiFiRegister::encryptionPowerStr(int8_t power) {
	if (power <= -100)
		return 0;
	else if (power >= -50)
		return 100;
	else
		return 2 * (power + 100);
}

const char * WiFiRegister::encryptionColorStr(int8_t power)
{
	if (power < -80)
		return PSTR("salmon");
	else if (power >= -50)
		return PSTR("lightgreen");
	else if (power >= -80 && power < -70)
		return PSTR("orange");
	else
		return PSTR("yellow");
}