#include "WebServerController.h"
using namespace std::placeholders;

WebServerController::WebServerController(uint8_t & position, uint8_t & destPosition) : _server(80), _webSocket(81), _position(position), _destPosition(destPosition), WiFiContr() {
	eeprom.getVar(1, _position);
	_destPosition = _position;
}

void WebServerController::beginOTA(const char * name, const char * pass) {
	ArduinoOTA.setHostname(name);
	ArduinoOTA.setPassword(pass);

	ArduinoOTA.onStart([]() {
		Serial.println(F("Start"));
	});
	ArduinoOTA.onEnd([]() {
		Serial.println(F("End\n"));
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.println(PSTR("Progress: ") + (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.print(F("Error: "));
		if (error == OTA_AUTH_ERROR)			Serial.println(F("Auth Failed"));
		else if (error == OTA_BEGIN_ERROR)		Serial.println(F("Begin Failed"));
		else if (error == OTA_CONNECT_ERROR)	Serial.println(F("Connect Failed"));
		else if (error == OTA_RECEIVE_ERROR)	Serial.println(F("Receive Failed"));
		else if (error == OTA_END_ERROR)		Serial.println(F("End Failed"));
	});
	ArduinoOTA.begin();
	yield();
	Serial.println(F("OTA ready"));
}

void WebServerController::beginSPIFFS() {
	SPIFFS.begin();
	Serial.println(F("\nSPIFFS started. Contents:"));
	{
		Dir dir = SPIFFS.openDir(F("/"));
		while (dir.next())
			Serial.println(PSTR("FS File: ") + dir.fileName() + PSTR(", size: ") + formatBytes(dir.fileSize()));
		Serial.println(F("End of content."));
	}
	yield();
}

void WebServerController::beginWebSocket() {
	_webSocket.begin();
	_webSocket.onEvent(std::bind(&WebServerController::webSocketEvent, this, _1, _2, _3, _4));
	yield();
	Serial.println(F("\nWebSocket server started."));
}

void WebServerController::beginServer() {
	_server.on(F("/edit.html"), HTTP_POST, std::bind(&WebServerController::handleFileUpload, this));
	_server.onNotFound(std::bind(&WebServerController::handleRoot, this));
	_server.begin();
	yield();
	Serial.println(F("\nHTML server started."));
}

void WebServerController::handleRoot()
{
	if (!handleFileRead(_server.uri()))
		_server.send(404, F("text/plain"), F("404: File Not Found"));
}

bool WebServerController::handleFileRead(String path) {
	//Serial.println("handleFileRead: " + path);
	if (path.endsWith(F("/")))
	{
		Serial.print(F("New connection! heap: "));
		Serial.println(ESP.getFreeHeap(), DEC);
		path += F("index.html");
	}
	String contentType = getContentType(path);
	String pathWithGz = path + F(".gz");
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
		if (SPIFFS.exists(pathWithGz))
			path += F(".gz");
		File file = SPIFFS.open(path, "r");
		size_t sent = _server.streamFile(file, contentType);
		file.close();
		Serial.println(PSTR("\tSent file: ") + path);
		return true;
	}
	Serial.println(PSTR("File Not Found: ") + path);
	return false;
}

void WebServerController::handleFileUpload() {
	_server.send(200, F("text/plain"), "");
	HTTPUpload& upload = _server.upload();
	String path;
	if (upload.status == UPLOAD_FILE_START) {
		path = upload.filename;
		if (!path.startsWith("/")) path = "/" + path;
		if (!path.endsWith(".gz")) {
			String pathWithGz = path + ".gz";
			if (SPIFFS.exists(pathWithGz))
				SPIFFS.remove(pathWithGz);
		}
		Serial.print(F("handleFileUpload Name: ")); Serial.println(path);
		_fsUploadFile = SPIFFS.open(path, "w");
		path = String();
	}
	else if (upload.status == UPLOAD_FILE_WRITE) {
		if (_fsUploadFile)
			_fsUploadFile.write(upload.buf, upload.currentSize);
	}
	else if (upload.status == UPLOAD_FILE_END) {
		if (_fsUploadFile) {
			_fsUploadFile.close();
			Serial.print(F("handleFileUpload Size: ")); Serial.println(upload.totalSize);
			_server.sendHeader(F("Location"), F("/success.html"));
			_server.send(303);
		}
		else {
			_server.send(500, F("text/plain"), F("500: couldn't create file"));
		}
	}
}

void WebServerController::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
	Serial.println(F("webSocketEvent"));
	switch (type) {
	case WStype_DISCONNECTED:
		Serial.print(num);
		Serial.println(F("Disconnected!"));
		break;
	case WStype_CONNECTED:
	{
		IPAddress ip = _webSocket.remoteIP(num), staticIP;
		Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
		uint8_t saveB, saveC;
		eeprom.getVar(1,saveB);
		eeprom.getVar(2, saveC);
		if (WiFi.getMode() == 2)
			staticIP = eeprom.getIp();
		else
			staticIP = WiFi.localIP();
		_webSocket.sendTXT(num, "I" + String(_position) + ',' + String(saveB) + ',' + String(saveC) + ',' + (eeprom.isStaticAddres() ? 'S' : 'D') + ',' + staticIP.toString());
		break;
	}
	case WStype_TEXT:
		Serial.printf("[%u] get Text: %s\r\n", num, payload);
		if (payload[0] == 'a') {
			_destPosition = String((char*)&payload[1]).toInt();
		}
		else if (payload[0] == 'b') {
			switch (payload[1])
			{
			case '1':
			{
				eeprom.setVar(1,_position);
				yield();
				_webSocket.broadcastTXT("B"+String(_position));
				break;
			}
			case '2':
			{
				eeprom.getVar(1, _destPosition);
				break;
			}
			case '3':
			{
				eeprom.setVar(2, _position);
				yield();
				_webSocket.broadcastTXT("C" + String(_position));
				break;
			}
			case '4':
			{
				eeprom.getVar(2, _destPosition);
				break;
			}
			case '5':
			{
				digitalWrite(4, HIGH);
				delay(300);
				digitalWrite(4, LOW);
				break;
			}
			case '6':
			{
				digitalWrite(5, HIGH);
				delay(300);
				digitalWrite(5, LOW);
				break;
			}
			case '7':
			{

				WiFiContr.changeMode();
				beginServer();
				beginWebSocket();
				break;
			}
			case '8':
			{
				WiFiContr.resetESP();
				break;
			}
			case '9':
			{
				ESP.deepSleep(0);
				break;
			}
			case '0':
			{
				if (eeprom.setStaticAddres(false))
					WiFiContr.resetESP();
				break;
			}
			default:
			{
				break;
			}
			}
		}
		else if(payload[0] == 'i')
		{
			IPAddress ip;
			ip.fromString(String((char*)&payload[1]));
			if (eeprom.setStaticAddres(true))
				if (eeprom.setIp(ip))
					WiFiContr.resetESP();
		}
		//_webSocket.broadcastTXT(payload, length);
		break;
	case WStype_BIN:
		Serial.println(F("Get binary"));
		hexdump(payload, length);
		_webSocket.sendBIN(num, payload, length);
		break;
	default:
		Serial.println(F("Invalid WStype "));
		break;
	}
}

String WebServerController::formatBytes(size_t bytes) {
	if (bytes < 1024)
		return String(bytes) + 'B';
	else if (bytes < (1024 * 1024))
		return String(bytes / 1024.0) + F("KB");
	else if (bytes < (1024 * 1024 * 1024))
		return String(bytes / 1024.0 / 1024.0) + F("MB");
}

String WebServerController::getContentType(String filename) {
	if (filename.endsWith(F(".html"))) return F("text/html");
	else if (filename.endsWith(F(".css"))) return F("text/css");
	else if (filename.endsWith(F(".js"))) return F("application/javascript");
	else if (filename.endsWith(F(".ico"))) return F("image/x-icon");
	else if (filename.endsWith(F(".gz"))) return F("application/x-gzip");
	return F("text/plain");
}

void WebServerController::webSocketLoop() {
	_webSocket.loop();
	yield();
}

void WebServerController::handleClientLoop() {
	_server.handleClient();
	if (WiFi.getMode() == 2)WiFiContr.dnsLoop();
	yield();
}

void WebServerController::otaLoop() {
	ArduinoOTA.handle();
	yield();
}

void WebServerController::webSocketSend(char sign, uint8_t num) {
	_webSocket.broadcastTXT(sign + String(num));
}



