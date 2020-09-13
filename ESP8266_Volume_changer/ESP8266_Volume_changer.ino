#include "WiFiController.h"
#include "WebServerController.h"
#include "reServo.h"

#define DNS_SSID "VolUP_WiFi"

uint32_t wiFiTimer(0), buttonsTimer(0), servoIsOnTimer(0), servoWebSocket(0);
uint8_t position, destPosition;
bool servoIsOn(false);

WebServerController WebServerContr(position,destPosition);
Servo servo;

void preinit() {
	ESP8266WiFiClass::preinitWiFiOff();
}

void setup()
{
	Serial.begin(115200);
	yield();

	Serial.println(ESP.getFreeHeap(), DEC);

	//WebServerContr.WiFiContr.forceWifiERegister();
	if (WebServerContr.WiFiContr.begin(DNS_SSID)) {
		WebServerContr.beginSPIFFS();
		WebServerContr.beginWebSocket();
		WebServerContr.beginServer();
	}

	pinMode(12, INPUT);
	pinMode(13, INPUT);

	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(0, OUTPUT);

	servo.attach(16);
}
void loop()
{
	yield();
	if (digitalRead(13) == HIGH)
		if (millis() - buttonsTimer >= 100) {
			buttonsTimer = millis();
			destPosition = position + 3 > 180 ? 180 : destPosition + 3;
		}

	if (digitalRead(12) == HIGH)
		if (millis() - buttonsTimer >= 100) {
			buttonsTimer = millis();
			destPosition = position - 3 < 0 ? 0 : destPosition - 3;
		}

	WebServerContr.webSocketLoop();
	WebServerContr.handleClientLoop();
	//WebServerContr.otaLoop();

	if (millis() - servoWebSocket >= 20)
	{
		servoWebSocket = millis();
		if (position != destPosition)
		{
			digitalWrite(0, HIGH); servoIsOn = true;
			if (destPosition > 180) destPosition = 180;
			if (destPosition < 0)	destPosition = 0;
			servo.write(destPosition);
			position = servo.read();
			WebServerContr.webSocketSend('A', position);
			Serial.println(destPosition);
			servoIsOnTimer = millis();

		}
	}

	if(servoIsOn)
		if (millis() - servoIsOnTimer >= 1000) {
			digitalWrite(0, LOW); servoIsOn = false;
		}

	if (millis() - wiFiTimer >= 660000) {
		wiFiTimer = millis();
		WebServerContr.WiFiContr.connect();
	}
}