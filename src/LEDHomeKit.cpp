//
// Created by Max Vissing on 2019-06-01.
//

#include "LEDHomeKit.h"

LEDHomeKit::LEDHomeKit() : hk(new ESPHomeKit()), wiFiSetup(nullptr)
#ifdef FAST_LED
, strip(new FastLEDStrip())
#else
, strip(new NeoPixelBusStrip())
#endif
#ifdef ALEXA_SUPPORT
, server(new ESP8266WebServer(80))
#endif
{
    strip->begin();
}

LEDHomeKit *LEDHomeKit::_instance = 0;

LEDHomeKit::~LEDHomeKit() {
    delete hk;
    delete wiFiSetup;
    delete strip;
}

void LEDHomeKit::setup() {
    auto accessory = new LEDAccessory("Max LED Strip", "LEDs", "2.0");
    hk->setAccessory(accessory);

    HKLOGINFO("[LEDHomeKit::setup] starting WiFi setup name: %s\r\n", hk->getName().c_str());
    wiFiSetup = new WiFiSetup(HKStorage::getSSID(), HKStorage::getWiFiPassword(), hk->getName(), std::bind(&LEDHomeKit::handleSSIDChange, this, std::placeholders::_1, std::placeholders::_2));
    wiFiSetup->start();

    hk->setup();
    
    #ifdef ALEXA_SUPPORT
    if (!alexaUdp.beginMulticast(WiFi.localIP(), IPAddress(239, 255, 255, 250), 1900)) {
        return;
    }
    server->onNotFound(std::bind(&LEDHomeKit::serveNotFound, this));
    server->on("/description.xml", HTTP_GET, std::bind(&LEDHomeKit::serveDescription, this));
    server->begin();
    #endif
}

void LEDHomeKit::update() {
    hk->update();
    wiFiSetup->update();
    
    #ifdef ALEXA_SUPPORT
    int len = alexaUdp.parsePacket();
    if (len <= 0) return;
    
    unsigned char data[len+1];
    alexaUdp.read(data, len);
    data[len] = 0;
    
    String request = (const char *) data;
    if (request.indexOf("M-SEARCH") >= 0) {
        if ((request.indexOf("ssdp:discover") > 0) || (request.indexOf("upnp:rootdevice") > 0) || (request.indexOf("device:basic:1") > 0)) {
            respondToAlexaSearch();
        }
    }
    server->handleClient();
    #endif
}

void LEDHomeKit::handleSSIDChange(const String &ssid, const String &password) {
    HKLOGINFO("[LEDHomeKit::handleSSIDChange] change SSID to %s\r\n", ssid.c_str());
    HKStorage::saveSSID(ssid);
    HKStorage::saveWiFiPassword(password);
}

void LEDHomeKit::handleReset() {
    HKLOGINFO("[LEDHomeKit::handleReset] handleReset\r\n");
    hk->reset();
}

void LEDHomeKit::resetPairings() {
    HKLOGINFO("[LEDHomeKit::resetPairings] resetPairings\r\n");
    hk->resetPairings();
}

LEDStrip *LEDHomeKit::getStrip() {
    return strip;
}

#ifdef ALEXA_SUPPORT
void LEDHomeKit::serveNotFound() {
    
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
            if (serveControl(req, body)) return;
        }
    }
    server->send(404, F("text/plain"), F("Not Found (espalexa)"));
}

bool LEDHomeKit::serveList(String url, String body) {
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
	char headers[strlen_P(HKALEXA_TCP_HEADERS) + 32];
	snprintf_P(
		headers, sizeof(headers),
		HKALEXA_TCP_HEADERS,
		"application/json", response.length()
	);
    server->sendContent(String(headers) + response);
    HKLOGDEBUG("[ALEXA] List request response: %s%s\r\n", headers, response.c_str());
    return true;
}

bool LEDHomeKit::serveControl(String url, String body) {
    if (body.indexOf("devicetype") > 0) {
        HKLOGDEBUG("[ALEXA] Handling devicetype request\r\n");
        String response = F("[{\"success\":{\"username\":\"2WLEDHardQrI3WHYTHoMcXHgEspsM8ZZRpSKtBQr\"}}]");
        char headers[strlen_P(HKALEXA_TCP_HEADERS) + 32];
        snprintf_P(
            headers, sizeof(headers),
            HKALEXA_TCP_HEADERS,
            "application/json", response.length()
        );
        server->sendContent(String(headers) + response);
        return true;
    }

    if ((url.indexOf("state") > 0) && (body.length() > 0)) {
        int pos = url.indexOf("lights");
        if (pos == -1) return false;
        HKLOGDEBUG("[ALEXA] Handling control request: %s\r\n", body.c_str());

        uint id = url.substring(pos+7).toInt();
        if (id > 0) {
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
                static_cast<LEDAccessory *>(hk->getAccessory())->setOn(selectedDevice, HKValue(FormatBool, false));
			} else {
                HKLOGDEBUG("[ALEXA] Mode \"%s\" start\r\n", selectedDevice->getName().c_str());
                static_cast<LEDAccessory *>(hk->getAccessory())->setOn(selectedDevice, HKValue(FormatBool, true));
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
                static_cast<LEDAccessory *>(hk->getAccessory())->setBrightness(selectedDevice, HKValue(FormatInt, value));
			}
			pos = body.indexOf("hue");
            if (pos > 0) {
                float value = body.substring(pos + 5).toInt();
                value *= 360.0/65535.0;
                HKLOGDEBUG("[ALEXA] Mode \"%s\" hue: %f orig: %u\r\n", selectedDevice->getName().c_str(), value, body.substring(pos + 5).toInt());
                static_cast<LEDAccessory *>(hk->getAccessory())->setHue(selectedDevice, HKValue(FormatFloat, value));
            }
			pos = body.indexOf("sat");
            if (pos > 0) {
                float value = body.substring(pos + 5).toInt();
                value *= 100.0/254.0;
                HKLOGDEBUG("[ALEXA] Mode \"%s\" saturation: %f orig: %u\r\n", selectedDevice->getName().c_str(), value, body.substring(pos + 5).toInt());
                static_cast<LEDAccessory *>(hk->getAccessory())->setSaturation(selectedDevice, HKValue(FormatFloat, value));
            }

			char response[strlen_P(HKALEXA_TCP_STATE_RESPONSE)+10];
			snprintf_P(
				response,
                sizeof(response),
				HKALEXA_TCP_STATE_RESPONSE,
				id+1,
                (static_cast<LEDAccessory *>(hk->getAccessory())->getOn(selectedDevice).boolValue)?"true":"false",
                id+1,
                brightness
			);

            char headers[strlen_P(HKALEXA_TCP_HEADERS) + 32];
            snprintf_P(
                headers, sizeof(headers),
                HKALEXA_TCP_HEADERS,
                "text/xml", strlen(response)
            );
            server->sendContent(String(headers) + response);
            HKLOGDEBUG("[ALEXA] Control request response: %s%s\r\n", headers, response);
			return true;
        }
    }
    return false;
}

String LEDHomeKit::deviceToJSON(LEDMode *mode, uint8_t id) {
    String type = F("Dimmable light");
    String modelid = F("LWB010");
    int typeId = 1;
    String colormode = "";
    char buf_col[strlen_P(HKALEXA_DEVICE_JSON_COLOR_TEMPLATE) + 8];
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
            HKALEXA_DEVICE_JSON_COLOR_TEMPLATE,
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

    char buf[strlen_P(HKALEXA_DEVICE_JSON_TEMPLATE) + strlen_P(HKALEXA_DEVICE_JSON_COLOR_TEMPLATE) + 100];
    snprintf_P(buf, 
        sizeof(buf),
        HKALEXA_DEVICE_JSON_TEMPLATE,
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

void LEDHomeKit::serveDescription() {
    IPAddress localIP = WiFi.localIP();
    char s[16];
    sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

    #if HKLOGLEVEL == 0
    IPAddress remoteIP = alexaUdp.remoteIP();
    HKLOGDEBUG("[LEDHomeKit::serveDescription] remoteIP: %d.%d.%d.%d:%u\r\n", remoteIP[0], remoteIP[1], remoteIP[2], remoteIP[3], alexaUdp.remotePort());
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

void LEDHomeKit::respondToAlexaSearch() {
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

    alexaUdp.beginPacket(alexaUdp.remoteIP(), alexaUdp.remotePort());
    alexaUdp.write(buf);
    alexaUdp.endPacket();           
}

String LEDHomeKit::byteToHex(uint8_t num) {
    String hstring = String(num, HEX);
    if (num < 16)
    {
        hstring = "0" + hstring;
    }

    return hstring;
}

String LEDHomeKit::makeMD5(String text) {
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
