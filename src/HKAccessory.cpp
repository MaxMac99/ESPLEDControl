//
// Created by Max Vissing on 2019-04-26.
//

#include "HKAccessory.h"


HKAccessory::HKAccessory(HKAccessoryCategory category) : id(0), category(category) {

}

void HKAccessory::addInfoService(const String& accName, const String& manufacturerName, const String& modelName, const String& serialNumber, const String &firmwareRevision) {
    auto *infoService = new HKService(HKServiceType::ServiceAccessoryInfo);
    addService(infoService);

    char *convName = (char *) malloc(accName.length()+1);
    strcpy(convName, accName.c_str());
    Serial.println("AccName: " + String(convName));
    HKValue nameValue{};
    nameValue.format = FormatString;
    nameValue.stringValue = convName;
    HKCharacteristic *nameChar = new HKCharacteristic(HKCharacteristicServiceName, nameValue, PermissionPairedRead, "Name", FormatString);
    infoService->addCharacteristic(nameChar);

    char *convManufacturer = (char *) malloc(manufacturerName.length()+1);
    strcpy(convManufacturer, manufacturerName.c_str());
    HKValue manufacturerValue{};
    manufacturerValue.format = FormatString;
    manufacturerValue.stringValue = convManufacturer;
    HKCharacteristic *manufacturerChar = new HKCharacteristic(HKCharacteristicManufactuer, manufacturerValue, PermissionPairedRead, "Manufacturer", FormatString);
    infoService->addCharacteristic(manufacturerChar);

    char *convModel = (char *) malloc(modelName.length()+1);
    strcpy(convModel, modelName.c_str());
    HKValue modelValue{};
    modelValue.format = FormatString;
    modelValue.stringValue = convModel;
    HKCharacteristic *modelChar = new HKCharacteristic(HKCharacteristicModelName, modelValue, PermissionPairedRead, "Model", FormatString);
    infoService->addCharacteristic(modelChar);

    char *convNumber = (char *) malloc(serialNumber.length()+1);
    strcpy(convNumber, serialNumber.c_str());
    HKValue serialValue{};
    serialValue.format = FormatString;
    serialValue.stringValue = convNumber;
    HKCharacteristic *serialChar = new HKCharacteristic(HKCharacteristicSerialNumber, serialValue, PermissionPairedRead, "Serial Number", FormatString);
    infoService->addCharacteristic(serialChar);

    /*
    char *convFirmwareRevision = (char *) malloc(firmwareRevision.length()+1);
    strcpy(convNumber, serialNumber.c_str());
    HKValue firmwareRevisionValue{};
    firmwareRevisionValue.format = FormatString;
    firmwareRevisionValue.stringValue = convFirmwareRevision;
    HKCharacteristic *firmwareRevisionChar = new HKCharacteristic(HKCharacteristicFirmwareRevision, firmwareRevisionValue, PermissionPairedRead, "Firmware Revision", FormatString);
    infoService->addCharacteristic(firmwareRevisionChar);*/

    HKValue identifyValue{};
    identifyValue.format = FormatBool;
    HKCharacteristic *identifyChar = new HKCharacteristic(HKCharacteristicIdentify, identifyValue, PermissionPairedWrite, "Identify", FormatBool);
    identifyChar->setSetter(std::bind(&HKAccessory::identify, this));
    infoService->addCharacteristic(identifyChar);
}

void HKAccessory::addService(HKService *service) {
    services.push_back(service);
}

void HKAccessory::identify() {
    Serial.println("Identify");
}

HKService *HKAccessory::getService(HKServiceType serviceType) {
    auto value = std::find_if(services.begin(), services.end(), [serviceType](HKService * const& obj){
        return obj->getServiceType() == serviceType;
    });
    if (value != services.end()) {
        return *value;
    }
    return nullptr;
}

unsigned int HKAccessory::getId() const {
    return id;
}

void HKAccessory::setId(unsigned int id) {
    HKAccessory::id = id;
}

void HKAccessory::setupServices() {
    unsigned int iid = 1;
    for (HKService *service : services) {
        if (service->getId() >= iid) {
            iid = service->getId()+1;
        } else {
            service->setId(iid++);
        }

        service->setupCharacteristics(iid);
    }
}

HKAccessoryCategory HKAccessory::getCategory() const {
    return category;
}

std::vector<HKService *> HKAccessory::getServices() {
    return services;
}

void HKAccessory::serializeToJSON(JSON &json, HKValue *value) {
    json.startObject();

    json.setString("aid");
    json.setInt(id);

    json.setString("services");
    json.startArray();

    for (auto service : services) {
        json.startObject();
        service->serializeToJSON(json, value);
        json.endObject();
    }

    json.endArray();

    json.endObject();
}

HKCharacteristic *HKAccessory::findCharacteristic(unsigned int id) {
    for (auto service : services) {
        if (HKCharacteristic *result = service->findCharacteristic(id)) {
            return result;
        }
    }
    return nullptr;
}
