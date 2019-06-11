//
// Created by Max Vissing on 2019-04-26.
//

#include "HKService.h"

HKService::HKService(HKServiceType type) : id(0), serviceType(type), hidden(false), primary(false) {

}

void HKService::addCharacteristic(HKCharacteristic *characteristic) {
    characteristics.push_back(characteristic);
}

HKCharacteristic *HKService::getCharacteristic(HKCharacteristicType characteristicType) {
    auto value = std::find_if(characteristics.begin(), characteristics.end(), [characteristicType](HKCharacteristic * const& obj) {
        return obj->getType() == characteristicType;
    });
    if (value != characteristics.end()) {
        return *value;
    }
    return nullptr;
}

HKServiceType HKService::getServiceType() const {
    return serviceType;
}

unsigned int HKService::getId() const {
    return id;
}

void HKService::setId(unsigned int id) {
    HKService::id = id;
}

void HKService::setupCharacteristics(unsigned int &iid) {
    for (HKCharacteristic *characteristic : characteristics) {
        if (characteristic->getId() >= iid) {
            iid = characteristic->getId()+1;
        } else {
            characteristic->setId(iid++);
        }
    }
}

std::vector<HKCharacteristic *> HKService::getCharacteristics() {
    return characteristics;
}

bool HKService::isHidden() {
    return hidden;
}

bool HKService::isPrimary() {
    return primary;
}

std::vector<HKService *> HKService::getLinkedServices() {
    return linked;
}

void HKService::serializeToJSON(JSON &json, HKValue *value) {
    json.setString("iid");
    json.setInt(id);

    json.setString("type");
    String typeConv = String(serviceType, HEX);
    typeConv.toUpperCase();
    json.setString(typeConv.c_str());

    /*
    json.setString("hidden");
    json.setBool(hidden);

    json.setString("primary");
    json.setBool(primary);*/

    if (!linked.empty()) {
        json.setString("linked");
        json.startArray();

        for (auto link : linked) {
            json.setInt(link->id);
        }

        json.endArray();
    }

    json.setString("characteristics");
    json.startArray();

    for (auto characteristic : characteristics) {
        json.startObject();
        characteristic->serializeToJSON(json, value);
        json.endObject();
    }

    json.endArray();
}

void HKService::setPrimary(bool primary) {
    HKService::primary = primary;
}

HKCharacteristic *HKService::findCharacteristic(unsigned int id) {
    for (auto ch : characteristics) {
        if (ch->getId() == id) {
            return ch;
        }
    }
    return nullptr;
}
