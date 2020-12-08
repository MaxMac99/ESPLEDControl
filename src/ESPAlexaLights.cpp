/**
 * @file ESPAlexaLights.cpp
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifdef ALEXA_SUPPORT

#include "ESPAlexaLights.h"

ESPAlexaLights::ESPAlexaLights(ESPHomeKit *hk) : hk(hk), server(new ESP8266WebServer(80)), udpServer() {
}

ESPAlexaLights::ESPAlexaLights(ESPHomeKit *hk, ESP8266WebServer *server) : hk(hk), server(server), udpServer() {
}

ESPAlexaLights::~ESPAlexaLights() {
    delete server;
}

void ESPAlexaLights::begin() {
    server->on("/description.xml", HTTP_GET, std::bind(&ESPAlexaLights::serveDescription, this));
    server->onNotFound(std::bind(&ESPAlexaLights::serveNotFound, this));
    if (udpServer.beginMulticast(WiFi.localIP(), IPAddress(239, 255, 255, 250), 1900)) {
        server->begin();
    }
}

void ESPAlexaLights::handle() {
    int len = udpServer.parsePacket();
    if (len <= 0) return;
    
    uint8_t *data = (uint8_t *) malloc(len+1);
    udpServer.read(data, len);
    data[len] = 0;
    
    String request = (const char *) data;
    if (request.indexOf("M-SEARCH") >= 0) {
        if ((request.indexOf("ssdp:discover") > 0) || (request.indexOf("upnp:rootdevice") > 0) || (request.indexOf("device:basic:1") > 0)) {
            free(data);
            serveUDPMessage();
        }
    }
    free(data);
    server->handleClient();
}

void ESPAlexaLights::serveUDPMessage() {
    IPAddress localIP = WiFi.localIP();
    char s[16];
    sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

    String escapedMac = WiFi.macAddress();
    escapedMac.replace(":", "");
    escapedMac.toLowerCase();
    char buf[1024];
    sprintf_P(buf,PSTR("HTTP/1.1 200 OK\r\n"
            "EXT:\r\n"
            "CACHE-CONTROL: max-age=100\r\n" // SSDP_INTERVAL
            "LOCATION: http://%s:80/description.xml\r\n"
            "SERVER: FreeRTOS/6.0.5, UPnP/1.0, IpBridge/1.17.0\r\n" // _modelName, _modelNumber
            "hue-bridgeid: %s\r\n"
            "ST: urn:schemas-upnp-org:device:basic:1\r\n"  // _deviceType
            "USN: uuid:2f402f80-da50-11e1-9b23-%s::upnp:rootdevice\r\n" // _uuid::_deviceType
            "\r\n"), s, escapedMac.c_str(), escapedMac.c_str());

    udpServer.beginPacket(udpServer.remoteIP(), udpServer.remotePort());
    udpServer.write(buf);
    udpServer.endPacket();           
}

void ESPAlexaLights::serveDescription() {
    IPAddress localIP = WiFi.localIP();
    char s[16];
    sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

    String escapedMac = WiFi.macAddress();
    escapedMac.replace(":", "");
    escapedMac.toLowerCase();

    char buf[1024];
    sprintf_P(buf, PSTR("<?xml version=\"1.0\" ?>"
        "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
            "<specVersion><major>1</major><minor>0</minor></specVersion>"
            "<URLBase>http://%s:80/</URLBase>"
            "<device>"
                "<deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType>"
                "<friendlyName>Espalexa (%s)</friendlyName>"
                "<manufacturer>Royal Philips Electronics</manufacturer>"
                "<manufacturerURL>http://www.philips.com</manufacturerURL>"
                "<modelDescription>Philips hue Personal Wireless Lighting</modelDescription>"
                "<modelName>Philips hue bridge 2012</modelName>"
                "<modelNumber>929000226503</modelNumber>"
                "<modelURL>http://www.meethue.com</modelURL>"
                "<serialNumber>%s</serialNumber>"
                "<UDN>uuid:2f402f80-da50-11e1-9b23-%s</UDN>"
                "<presentationURL>index.html</presentationURL>"
            "</device>"
        "</root>"), 
        s, 
        s, 
        escapedMac.c_str(), 
        escapedMac.c_str());
        
    server->send(200, "text/xml", buf);
}

void ESPAlexaLights::serveNotFound() {
    String req = server->uri();
    String body = server->arg(0);
    
    if (req.startsWith("/api")) {
        if (server->method() == HTTP_GET) {
            if (serveList(req, body)) return;
        } else {
            if (body.indexOf("devicetype") > 0) {
                if (serveDeviceType()) return;
            } else {
                if (serveControl(req, body)) return;
            }
        }
    }
    server->send(404, F("text/plain"), F("Not Found (ESPAlexaLights)"));
}

bool ESPAlexaLights::serveList(const String &url, const String &body) {
    int pos = url.indexOf("lights");
    if (pos == -1) return false;

    uint id = url.substring(pos+7).toInt();
    String response;
    if (id == 0) {
        response += "{";
        uint i = 0;
        for (auto service : hk->getAccessory()->getServices()) {
            if (service->getClassId() >= LEDMODE_CLASS_ID) {
                if (i > 0) {
                    response += ",";
                }
                response += "\"" + String(i+1) + "\":" + deviceToJSON(static_cast<LEDMode *>(service), i);
                i++;
            }
        }
        response += "}";
    } else {
        uint i = 0;
        for (auto service : hk->getAccessory()->getServices()) {
            if (service->getClassId() >= LEDMODE_CLASS_ID) {
                if (i == id-1) {
                    response = deviceToJSON(static_cast<LEDMode *>(service), i);
                    break;
                }
                i++;
            }
        }
    }
	char headers[strlen_P(ESPALEXA_TCP_HEADERS) + 32];
    
	snprintf_P(
		headers, sizeof(headers),
		ESPALEXA_TCP_HEADERS,
		"application/json", response.length()
	);
    server->sendContent(String(headers) + response);
    return true;
}

bool ESPAlexaLights::serveControl(const String &url, const String &body) {
    if (!((url.indexOf("state") > 0) && (body.length() > 0))) return false;

    int pos = url.indexOf("lights");
    if (pos == -1) return false;

    uint id = url.substring(pos+7).toInt();
    if (id <= 0) return false;

    id--;
    LEDMode *selectedDevice = nullptr;
    uint i = 0;
    for (auto service : hk->getAccessory()->getServices()) {
        if (service->getClassId() >= LEDMODE_CLASS_ID) {
            if (i == id) {
                selectedDevice = static_cast<LEDMode *>(service);
            }
            i++;
        }
    }
    if (selectedDevice == nullptr) {
        return false;
    }

    if (body.indexOf("false") > 0) {
        HKLOGDEBUG("[ALEXA] Mode \"%s\" stop\r\n", selectedDevice->getName().c_str());
        static_cast<LEDAccessory *>(hk->getAccessory())->setOn(selectedDevice, HKValue(HKFormatBool, false));
    } else {
        HKLOGDEBUG("[ALEXA] Mode \"%s\" start\r\n", selectedDevice->getName().c_str());
        static_cast<LEDAccessory *>(hk->getAccessory())->setOn(selectedDevice, HKValue(HKFormatBool, true));
    }
    pos = body.indexOf("bri");
    int brightness = static_cast<LEDAccessory *>(hk->getAccessory())->getBrightness(selectedDevice).intValue*255;
    brightness /= 100;
    if (brightness >= 255 || brightness == 0) {
        brightness = 254;
    }
    if (pos > 0) {
        unsigned char value = body.substring(pos+5).toInt();
        brightness = value;
        if (value >= 254) {
            value = 100;
        } else {
            value *= 100.0/254.0;
        }
        HKLOGDEBUG("[ALEXA] Mode \"%s\" brightness: %u orig: %u\r\n", selectedDevice->getName().c_str(), value, body.substring(pos + 5).toInt());
        static_cast<LEDAccessory *>(hk->getAccessory())->setBrightness(selectedDevice, HKValue(HKFormatInt, value));
    }
    pos = body.indexOf("hue");
    if (pos > 0) {
        float value = body.substring(pos + 5).toInt();
        value *= 360.0/65535.0;
        HKLOGDEBUG("[ALEXA] Mode \"%s\" hue: %f orig: %u\r\n", selectedDevice->getName().c_str(), value, body.substring(pos + 5).toInt());
        static_cast<LEDAccessory *>(hk->getAccessory())->setHue(selectedDevice, HKValue(HKFormatFloat, value));
    }
    pos = body.indexOf("sat");
    if (pos > 0) {
        float value = body.substring(pos + 5).toInt();
        value *= 100.0/254.0;
        HKLOGDEBUG("[ALEXA] Mode \"%s\" saturation: %f orig: %u\r\n", selectedDevice->getName().c_str(), value, body.substring(pos + 5).toInt());
        static_cast<LEDAccessory *>(hk->getAccessory())->setSaturation(selectedDevice, HKValue(HKFormatFloat, value));
    }

    char response[strlen_P(ESPALEXA_TCP_STATE_RESPONSE)+10];
    snprintf_P(
        response,
        sizeof(response),
        ESPALEXA_TCP_STATE_RESPONSE,
        id+1,
        (static_cast<LEDAccessory *>(hk->getAccessory())->getOn(selectedDevice).boolValue)?"true":"false",
        id+1,
        brightness
    );

    char headers[strlen_P(ESPALEXA_TCP_HEADERS) + 32];
    snprintf_P(
        headers, sizeof(headers),
        ESPALEXA_TCP_HEADERS,
        "text/xml", strlen(response)
    );
    server->sendContent(String(headers) + response);
    return true;
}

bool ESPAlexaLights::serveDeviceType() {
    String response = F("[{\"success\":{\"username\":\"2WLEDHardQrI3WHYTHoMcXHgEspsM8ZZRpSKtBQr\"}}]");
    char headers[strlen_P(ESPALEXA_TCP_HEADERS) + 32];
    snprintf_P(
        headers, sizeof(headers),
        ESPALEXA_TCP_HEADERS,
        "application/json", response.length()
    );
    server->sendContent(String(headers) + response);
    return true;
}

String ESPAlexaLights::deviceToJSON(LEDMode *mode, uint8_t id) {
    String type = F("Dimmable light");
    String modelid = F("LWB010");
    int typeId = 1;
    String colormode = "";
    char buf_col[strlen_P(ESPALEXA_DEVICE_JSON_COLOR_TEMPLATE) + 8];
    buf_col[0] = 0;
    if (mode->getCharacteristic(HKCharacteristicHue)) {
        int hue = static_cast<LEDAccessory *>(hk->getAccessory())->getHue(mode).floatValue * (65535.0/360.0);
        if (hue > 65535) {
            hue %= 65535;
        }
        int saturation = (int)static_cast<LEDAccessory *>(hk->getAccessory())->getSaturation(mode).floatValue * 255;
        saturation /= 100;
        if (saturation >= 255) {
            saturation = 254;
        }

        type = F("Color light");
        modelid = F("LST001");
        typeId = 3;
        snprintf_P(buf_col,
            sizeof(buf_col),
            ESPALEXA_DEVICE_JSON_COLOR_TEMPLATE,
            hue, 
            saturation);
        
        colormode = F("\"colormode\":\"hs\",");
    }
    int brightness = static_cast<LEDAccessory *>(hk->getAccessory())->getBrightness(mode).intValue*255;
    brightness /= 100;
    if (brightness >= 255 || brightness == 0) {
        brightness = 254;
    }
    
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    mac.toLowerCase();
    mac.concat(mode->getName());
    String uniqueid = makeMD5(mac).substring(0, 12);

    char buf[strlen_P(ESPALEXA_DEVICE_JSON_TEMPLATE) + strlen_P(ESPALEXA_DEVICE_JSON_COLOR_TEMPLATE) + 100];
    snprintf_P(buf, 
        sizeof(buf),
        ESPALEXA_DEVICE_JSON_TEMPLATE,
        type.c_str(),
        mode->getName().c_str(),
        uniqueid.c_str(),
        modelid.c_str(),
        typeId,
        (static_cast<LEDAccessory *>(hk->getAccessory())->getOn(mode).boolValue)?"true":"false",
        brightness,
        buf_col,
        colormode.c_str()
    );
    return String(buf);
}

String ESPAlexaLights::byteToHex(uint8_t num) {
    String hstring = String(num, HEX);
    if (num < 16)
    {
        hstring = "0" + hstring;
    }

    return hstring;
}

String ESPAlexaLights::makeMD5(const String &text) {
    unsigned char bbuf[16];
    String hash = "";
    MD5Builder md5;
    md5.begin();
    md5.add(text);
    md5.calculate();
    
    md5.getBytes(bbuf);
    for (uint8_t i = 0; i < 16; i++)
    {
        hash += byteToHex(bbuf[i]);
    }

    return hash;
}

#endif
