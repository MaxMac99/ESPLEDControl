//
// Created by Max Vissing on 2019-04-16.
//

#ifndef LEDCONTROLLER_WIFISETUP_H
#define LEDCONTROLLER_WIFISETUP_H

#include <Arduino.h>

#define REFETCH_INTERVAL 10000

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

const char HTTP_HEAD[] PROGMEM = "<!DOCTYPE html>\n"
                                 "<html lang=\"en\">\n"
                                 "<head>\n"
                                 "\t<meta charset=\"UTF-8\">\n"
                                 "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                                 "\t<meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n"
                                 "\t<title>{v}</title>\n";
const char HTTP_STYLE[] PROGMEM = "\t<style type=\"text/css\">\n"
                                  "\t\thtml,\n"
                                  "\t\tbody {\n"
                                  "\t\t\tfont-family: sans-serif;\n"
                                  "\t\t\tmargin: 0;\n"
                                  "\t\t\tpadding: 0;\n"
                                  "\t\t\tfont-size: 10px;\n"
                                  "\t\t\tbackground: #EEEFF3;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.bar {\n"
                                  "\t\t\tposition: absolute;\n"
                                  "\t\t\twidth: 100%;\n"
                                  "\t\t\tbackground: #FFF;\n"
                                  "\t\t\tcolor: #000;\n"
                                  "\t\t\ttext-align: center;\n"
                                  "\t\t\tfont-size: 2rem;\n"
                                  "\t\t\tpadding: 1rem 0;\n"
                                  "\t\t\tfont-weight: bold;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.main {\n"
                                  "\t\t\tposition: absolute;\n"
                                  "\t\t\ttop: 5rem;\n"
                                  "\t\t\tleft: 50%;\n"
                                  "\t\t\twidth: 31.5rem;\n"
                                  "\t\t\ttransform: translateX(-50%);\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.ssids p {\n"
                                  "\t\t\tmargin: 0;\n"
                                  "\t\t\tpadding: 0.2rem;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.ssid,\n"
                                  "\t\t.other,\n"
                                  "\t\t.input {\n"
                                  "\t\t\tposition: relative;\n"
                                  "\t\t\tleft: 50%;\n"
                                  "\t\t\twidth: 25rem;\n"
                                  "\t\t\tbackground: #FFF;\n"
                                  "\t\t\tborder: 0.1rem solid #CCC;\n"
                                  "\t\t\tpadding: 1.5rem 10%;\n"
                                  "\t\t\ttransition: all 250ms;\n"
                                  "\t\t\ttransform: translateX(-50%);\n"
                                  "\t\t\tfont-size: 12px;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.ssid,\n"
                                  "\t\t.input {\n"
                                  "\t\t\tmargin-top: -0.1rem;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.ssid p,\n"
                                  "\t\t.ssid #wifi {\n"
                                  "\t\t\tmargin: 0;\n"
                                  "\t\t\tpadding: 0;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.ssid #lock {\n"
                                  "\t\t\tfloat: left;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.ssid #wifi {\n"
                                  "\t\t\tfloat: right;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.other {\n"
                                  "\t\t\tmargin-bottom: 1.5rem;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\ta {\n"
                                  "\t\t\ttext-decoration: none;\n"
                                  "\t\t\tdisplay: block;\n"
                                  "\t\t\tcursor: pointer;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.other p {\n"
                                  "\t\t\tcolor: #000;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.other:hover,\n"
                                  "\t\t.ssid:hover {\n"
                                  "\t\t\tbackground: #CCC;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.new p {\n"
                                  "\t\t\tmargin: 0;\n"
                                  "\t\t\tpadding: 0.2rem;\n"
                                  "\t\t\twidth: 25rem;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\tinput {\n"
                                  "\t\t\twidth: 100%;\n"
                                  "\t\t\tborder: none;\n"
                                  "\t\t\tbackground: transparent;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.submit {\n"
                                  "\t\t\tmargin-top: -0.1rem;\n"
                                  "\t\t\tcolor: #127CC9;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.hover:hover,\n"
                                  "\t\t.hover:hover>button {\n"
                                  "\t\t\tbackground: #CCC;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.hide {\n"
                                  "\t\t\tdisplay: none !important;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.done{\n"
                                  "\t\t\tfont-size: 2.5rem;\n"
                                  "\t\t\ttext-align: center;\n"
                                  "\t\t}\n"
                                  "\t\t\n"
                                  "\t\t.error{\n"
                                  "\t\t\twidth: 28.2rem;\n"
                                  "\t\t\tpadding: 1.5rem;\n"
                                  "\t\t\tmargin: 1.5rem 0;\n"
                                  "\t\t\tfont-size: 1.5rem;\n"
                                  "\t\t\tcolor: #FFF;\n"
                                  "\t\t\tdisplay: block;\n"
                                  "\t\t\tbackground: rgba(255,0,0, 0.5);\n"
                                  "\t\t\tborder: 0.1rem solid #F00;\n"
                                  "\t\t\tborder-radius: 0.5rem;\n"
                                  "\t\t\ttext-align: center;\n"
                                  "\t\t\ttransition: all 250ms;\n"
                                  "\t\t}\n"
                                  "\t\t.error:hover{\n"
                                  "\t\t\tbackground: rgba(255,0,0, 0.8);\n"
                                  "\t\t}\n"
                                  "\n"
                                  "\t\t.load{\n"
                                  "\t\t\tposition: absolute;\n"
                                  "\t\t\tz-index: 10;\n"
                                  "\t\t\ttop: 4rem;\n"
                                  "\t\t\tleft: 0;\n"
                                  "\t\t\tdisplay: block;\n"
                                  "\t\t\twidth: 100%;\n"
                                  "\t\t\theight: 100%;\n"
                                  "\t\t\tbackground: rgba(0, 0, 0, 0.7);\n"
                                  "\t\t}\n"
                                  "\t\t.loader {\n"
                                  "\t\t\tposition: fixed;\n"
                                  "\t\t\tleft: 50%;\n"
                                  "\t\t\ttop: 50%;\n"
                                  "\t\t\tborder: 1rem solid #FFF;\n"
                                  "\t\t\tborder-top: 1rem solid #CCC;\n"
                                  "\t\t\tborder-radius: 50%;\n"
                                  "\t\t\twidth: 5rem;\n"
                                  "\t\t\theight: 5rem;\n"
                                  "\t\t\tanimation: spin 2s linear infinite;\n"
                                  "\t\t\ttransform: translateX(-50%) translateY(-50%);\n"
                                  "\t\t}\n"
                                  "\t\t@keyframes spin {\n"
                                  "\t\t\t0% { transform: translateX(-50%) translateY(-50%) rotate(0deg); }\n"
                                  "\t\t\t100% { transform: translateX(-50%) translateY(-50%) rotate(360deg); }\n"
                                  "\t\t}\n"
                                  "\t</style>\n";
const char HTTP_SCRIPT[] PROGMEM = "\t<script>\n"
                                   "\t\tfunction otherNetwork() {\n"
                                   "\t\t\tdocument.getElementById(\"new\").classList.toggle(\"hide\");\n"
                                   "\t\t\tdocument.getElementById(\"ssids\").classList.toggle(\"hide\");\n"
                                   "\t\t\tvar ssidInput = document.getElementById(\"ssid_input\");\n"
                                   "\t\t\tif (ssidInput.value == \"\") {\n"
                                   "\t\t\t\tssidInput.focus();\n"
                                   "\t\t\t} else {\n"
                                   "\t\t\t\tdocument.getElementById(\"password_input\").focus();\n"
                                   "\t\t\t}\n"
                                   "\t\t\tcheckInputs();\n"
                                   "\t\t}\n"
                                   "\n"
                                   "\t\tfunction selectSSID(item) {\n"
                                   "\t\t\tdocument.getElementById(\"ssid_input\").value = item.getElementsByClassName(\"name\")[0].textContent;\n"
                                   "\t\t\totherNetwork();\n"
                                   "\t\t\tcheckInputs();\n"
                                   "\t\t}\n"
                                   "\n"
                                   "\t\tfunction checkInputs() {\n"
                                   "\t\t\tvar ssid = document.getElementById(\"ssid_input\").value;\n"
                                   "\t\t\tvar pwd = document.getElementById(\"password_input\").value;\n"
                                   "\t\t\tif (ssid == \"\" || pwd == \"\") {\n"
                                   "\t\t\t\tdocument.getElementById(\"connect\").style = \"cursor:default; color: dimgray; transition: none; background: #FFF\";\n"
                                   "\t\t\t\treturn false;\n"
                                   "\t\t\t} else {\n"
                                   "\t\t\t\tdocument.getElementById(\"connect\").style = \"\";\n"
                                   "\t\t\t\treturn true;\n"
                                   "\t\t\t}\n"
                                   "\t\t}\n"
                                   "\n"
                                   "\t\tfunction connect() {\n"
                                   "\t\t\tdocument.getElementById('load').classList.toggle(\"hide\");\n"
                                   "\t\t\tif (checkInputs()) {\n"
                                   "\t\t\t\tvar xhttp = new XMLHttpRequest();\n"
                                   "\t\t\t\txhttp.timeout = 20000;\n"
                                   "\t\t\t\txhttp.onreadystatechange = function() {\n"
                                   "\t\t\t\t\tif (this.readyState == 4) {\n"
                                   "\t\t\t\t\t\tdocument.getElementById('load').classList.toggle('hide');\n"
                                   "\t\t\t\t\t\tif (this.status == 200) {\n"
                                   "\t\t\t\t\t\t\tdocument.getElementById(\"ssids\").remove();\n"
                                   "\t\t\t\t\t\t\tdocument.getElementById(\"new\").remove();\n"
                                   "\t\t\t\t\t\t\tdocument.getElementsByClassName(\"main\")[0].innerHTML = '<p class=\"done\">Verbunden<span>&#10003;</span></p>';\n"
                                   "\t\t\t\t\t\t} else {\n"
                                   "\t\t\t\t\t\t\tvar main = document.getElementsByClassName(\"main\")[0];\n"
                                   "\t\t\t\t\t\t\tvar errors = main.getElementsByClassName(\"error\");\n"
                                   "\t\t\t\t\t\t\tArray.prototype.forEach.call(errors, function(element) {\n"
                                   "\t\t\t\t\t\t\t\telement.remove();\n"
                                   "\t\t\t\t\t\t\t});\n"
                                   "\t\t\t\t\t\t\tmain.innerHTML = '<div class=\"error\">' + this.responseText + '</div>' + main.innerHTML;\n"
                                   "\t\t\t\t\t\t}\n"
                                   "\t\t\t\t\t}\n"
                                   "\t\t\t\t};\n"
                                   "\n"
                                   "\t\t\t\tvar params = \"ssid=\" + encodeURIComponent(document.getElementById(\"ssid_input\").value);\n"
                                   "\t\t\t\tparams += \"&password=\" + encodeURIComponent(document.getElementById(\"password_input\").value);\n"
                                   "\t\t\t\txhttp.open(\"POST\", \"/wifisave\", true);\n"
                                   "\t\t\t\txhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');\n"
                                   "\t\t\t\txhttp.send(params);\n"
                                   "\t\t\t}\n"
                                   "\t\t}\n"
                                   "\t</script>";
const char HTTP_HEAD_END[] PROGMEM = "<body>\n"
                                     "\t<div class=\"bar\">\n"
                                     "\t\tWLAN LED STRIPE\n"
                                     "\t</div>\n"
                                     "\t<div id=\"load\" class=\"load hide\">\n"
                                     "\t\t<div class=\"loader\"></div>\n"
                                     "\t</div>\n"
                                     "\t<div class=\"main\">\n";
const char HTTP_SSIDS_START[] PROGMEM = "\t\t<div id=\"ssids\" class=\"ssids\">\n"
                                        "\t\t\t<p>Netzwerk wählen ...</p>\n";
const char HTTP_SSID_ITEM[] PROGMEM = "\t\t\t<a class=\"ssid\" onclick=\"selectSSID(this);\">\n"
                                      "\t\t\t\t<p>\n"
                                      "\t\t\t\t\t<span class=\"lock\">{i}</span>&nbsp;\n"
                                      "\t\t\t\t\t<span class=\"name\">{v}</span>\n"
                                      "\t\t\t\t\t<span class=\"wifi\">{r}</span>\n"
                                      "\t\t\t\t</p>\n"
                                      "\t\t\t</a>\n";
const char HTTP_SSIDS_END[] PROGMEM = "\t\t\t<br>\n"
                                      "\t\t\t<a id=\"other\" class=\"other\" onclick=\"otherNetwork();\">\n"
                                      "\t\t\t\t<p>Anderes ...</p>\n"
                                      "\t\t\t</a>\n"
                                      "\t\t</div>\n";
const char HTTP_FORM[] PROGMEM = "\t\t<div id=\"new\" class=\"new hide\">\n"
                                 "\t\t\t<form name=\"wifi-form\" method=\"get\" action=\"wifisave\">\n"
                                 "\t\t\t\t<p>Daten eingeben ...</p>\n"
                                 "\t\t\t\t<div class=\"input\">\n"
                                 "\t\t\t\t\t<input id=\"ssid_input\" name=\"ssid\" type=\"text\" placeholder=\"SSID\" oninput=\"checkInputs();\" />\n"
                                 "\t\t\t\t</div>\n"
                                 "\t\t\t\t<div class=\"input\">\n"
                                 "\t\t\t\t\t<input id=\"password_input\" name=\"password\" type=\"password\" placeholder=\"Passwort\" oninput=\"checkInputs();\" />\n"
                                 "\t\t\t\t</div>\n"
                                 "\t\t\t\t<br>\n"
                                 "\t\t\t\t<a class=\"other submit\" id=\"connect\" onclick=\"connect();\">\n"
                                 "\t\t\t\t\tVerbinden\n"
                                 "\t\t\t\t</a>\n"
                                 "\t\t\t\t<br>\n"
                                 "\t\t\t\t<a id=\"back\" class=\"other\" onclick=\"otherNetwork();\">\n"
                                 "\t\t\t\t\t<p>Zurück ...</p>\n"
                                 "\t\t\t\t</a>\n"
                                 "\t\t\t</form>\n"
                                 "\t\t</div>\n";
const char HTTP_END[] PROGMEM = "\t</div>\n"
                                "</body>\n"
                                "</html>";


class WiFiSetup {
public:
    WiFiSetup();
    WiFiSetup(String ssid, String password, String hostname,
              std::function<void(String, String)> callback);
    bool start();
    void update();

    void setSsid(const String &ssid);

    void setPassword(const String &password);

    void setHostname(const String &hostname);

    void setChangeCallback(const std::function<void(String, String)> &changeCallback);

private:
    String ssid;
    String password;
    String hostname;
    std::function<void(String, String)> changeCallback;
    bool connected;
    unsigned long lastUpdate;

    std::unique_ptr<ESP8266WebServer> server;
    std::unique_ptr<DNSServer> dns;
private:
    bool startConfigPortal();
    void setupConfigPortal(const String& apName);
    void handleRoot();
    void handleWifiSave();
    void handleNotFound();
    static uint8_t connectWifi(const String& ssid, const String& password);
    bool captivePortal();
    static bool isIp(const String& str);
    static String toStringIp(const IPAddress& ip);
};


#endif //LEDCONTROLLER_WIFISETUP_H