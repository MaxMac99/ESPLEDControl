//
// Created by Max Vissing on 2019-04-26.
//

#include "HKService.h"

HKService::HKService(HKServiceType type) : id(0), serviceType(type) {

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
