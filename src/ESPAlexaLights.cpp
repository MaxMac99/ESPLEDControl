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
    if (len > 0) {
        uint8_t *data = (uint8_t *) malloc(len+1);
        udpServer.read(data, len);
        data[len] = 0;
        
        String request = String((char *) data);
        if (request.indexOf("M-SEARCH") >= 0) {
            if ((request.indexOf("ssdp:discover") > 0) || (request.indexOf("upnp:rootdevice") > 0) || (request.indexOf("device:basic:1") > 0)) {
                serveUDPMessage();
            }
        }
        free(data);
    }
    
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

    #if HKLOGLEVEL == 0
    IPAddress remoteIP = udpServer.remoteIP();
    HKLOGDEBUG("[LEDHomeKit::serveDescription] remoteIP: %d.%d.%d.%d:%u\r\n", remoteIP[0], remoteIP[1], remoteIP[2], remoteIP[3], udpServer.remotePort());
    #endif

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

    #if HKLOGLEVEL == 0
    IPAddress remoteIP = server->client().remoteIP();
    HKLOGDEBUG("[LEDHomeKit::serveNotFound] ip: %s url: %s body: %s\r\n", remoteIP.toString().c_str(), req.c_str(), body.c_str());
    #endif
    
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
    HKLOGDEBUG("[ALEXA] Handling list request id: %u\r\n", id);
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
    char *headers = (char *) malloc(strlen_P(ESPALEXA_TCP_HEADERS) + 32);
    
	sprintf_P(
		headers,
		ESPALEXA_TCP_HEADERS,
		PSTR("application/json"), response.length()
	);
    HKLOGDEBUG("[ALEXA] Response: %s\r\n", response.c_str());
    server->sendContent(String(headers) + response);
    free(headers);
    return true;
}

bool ESPAlexaLights::serveControl(const String &url, const String &body) {
    if (!((url.indexOf("state") > 0) && (body.length() > 0))) return false;

    int pos = url.indexOf("lights");
    if (pos == -1) return false;
    HKLOGDEBUG("[ALEXA] Handling control request: %s\r\n", body.c_str());

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

    char *response = (char *) malloc(strlen_P(ESPALEXA_TCP_STATE_RESPONSE)+10);
    sprintf_P(
        response,
        ESPALEXA_TCP_STATE_RESPONSE,
        id+1,
        (static_cast<LEDAccessory *>(hk->getAccessory())->getOn(selectedDevice).boolValue)?"true":"false",
        id+1,
        brightness
    );

    char *headers = (char *) malloc(strlen_P(ESPALEXA_TCP_HEADERS) + 32);
    sprintf_P(
        headers,
        ESPALEXA_TCP_HEADERS,
        PSTR("text/xml"), strlen(response)
    );
    server->sendContent(String(headers) + response);
    HKLOGDEBUG("[ALEXA] Control request response: %s%s\r\n", headers, response);
    free(response);
    free(headers);
    return true;
}

bool ESPAlexaLights::serveDeviceType() {
    HKLOGDEBUG("[ALEXA] Handling devicetype request\r\n");
    char *headers = (char *) malloc(sizeof(ESPALEXA_TCP_HEADERS_WITH_CONTENT) + 32 + strlen_P(ESPALEXA_DEVICE_TYPE_RESPONSE));
    size_t currentSize = sprintf_P(
        headers,
        ESPALEXA_TCP_HEADERS_WITH_CONTENT,
        PSTR("application/json"), strlen_P(ESPALEXA_DEVICE_TYPE_RESPONSE),
        ESPALEXA_DEVICE_TYPE_RESPONSE
    );
    server->sendContent(headers, currentSize);
    free(headers);
    return true;
}

String ESPAlexaLights::deviceToJSON(LEDMode *mode, uint8_t id) {
    String type = F("Dimmable light");
    String modelid = F("LWB010");
    int typeId = 1;
    String colormode = "";
    
    char *buf_col = (char *) malloc(strlen_P(ESPALEXA_DEVICE_JSON_COLOR_TEMPLATE) + 8);
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
        sprintf_P(buf_col,
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

    char *buf = (char *) malloc(strlen_P(ESPALEXA_DEVICE_JSON_TEMPLATE) + strlen_P(ESPALEXA_DEVICE_JSON_COLOR_TEMPLATE) + 100);
    sprintf_P(buf,
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
    free(buf_col);
    String json = String(buf);
    free(buf);
    return json;
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
