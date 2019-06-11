//
// Created by Max Vissing on 2019-04-26.
//

#define LWIP_INTERNAL

#include "HKServer.h"
#include "lwip/tcp.h"
#include <include/ClientContext.h>

HKServer::HKServer(HomeKit *hk) : WiFiServer(PORT), hk(hk), mdnsService() {
}

void HKServer::setup() {
    setupMDNS();

    begin();
}

void HKServer::update() {
    MDNS.update();

    HKClient *newClient = availableHK();
    if (newClient && *newClient) {
        if (clients.size() >= MAX_CLIENTS) {
            return;
        }

        Serial.println("New Client connected (" + String(newClient->remoteIP().toString()) + ":" + String(newClient->remotePort()) + ")");

        newClient->setTimeout(10000);
        newClient->keepAlive(180, 30, 4);
        clients.push_back(newClient);
    } else {
        delete newClient;
    }

    for (auto it = clients.begin(); it != clients.end();) {
        auto client = *it;
        if (client->available()) {
            client->received();
        }

        if (!client->connected()) {
            Serial.println("Client disconnected");
            delete *it;
            it = clients.erase(it);
        } else {
            it++;
        }
    }
}

bool HKServer::isPairing() {
    for (auto client : clients) {
        if (client->pairing) {
            return true;
        }
    }
    return false;
}

HKClient *HKServer::availableHK(byte* status) {
    (void) status;
    if (_unclaimed) {
        auto result = new HKClient(this, _unclaimed);
        _unclaimed = _unclaimed->next();
        result->setNoDelay(getNoDelay());
        DEBUGV("WS:av\r\n");
        return result;
    }

    optimistic_yield(1000);
    return nullptr;
}

int HKServer::setupMDNS() {
    Serial.println("Setup mDNS");
    String uniqueName = hk->getName();

    if (!mdnsService && !MDNS.begin(uniqueName)) {
        Serial.println("Failed to begin mDNS");
        return -1;
    }
    
    const char *service = "hap";
    const char *protocol = "tcp";
    MDNS.setInstanceName(uniqueName);
    if (!mdnsService) {
        mdnsService = MDNS.addService(nullptr, service, protocol, PORT);
        if (!mdnsService) {
            Serial.println("Failed to add service");
            return -1;
        }
    }

    HKService *info = hk->getAccessory()->getService(ServiceAccessoryInfo);
    if (info == nullptr) {
        return -1;
    }
    HKCharacteristic *model = info->getCharacteristic(HKCharacteristicModelName);
    if (model == nullptr) {
        return -1;
    }

    if (!MDNS.addServiceTxt(service, protocol, "md", model->getValue().stringValue)) {
        Serial.println("Failed to add model");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "pv", "1.0")) {
        Serial.println("Failed to add \"1.0\"");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "id", hk->getAccessoryId())) {
        Serial.println("Failed to add accessoryId");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "c#", String(hk->getConfigNumber()))) {
        Serial.println("Failed to add configNumber");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "s#", "1")) {  // State number
        Serial.println("Failed to 1");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "ff", "0")) {  // feature flags
        //   bit 0 - supports HAP pairing. required for all HomeKit accessories
        //   bits 1-7 - reserved
        Serial.println("Failed to ");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "sf", String(HKStorage::isPaired() ? 0 : 1))) {  // status flags
        //   bit 0 - not paired
        //   bit 1 - not configured to join WiFi
        //   bit 2 - problem detected on accessory
        //   bits 3-7 - reserved
        Serial.println("Failed to ");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "ci", String(hk->getAccessory()->getCategory()))) {
        Serial.println("Failed to ");
        return false;
    }

    // setupId
    return true;
}
