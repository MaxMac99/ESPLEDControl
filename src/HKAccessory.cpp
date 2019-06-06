//
// Created by Max Vissing on 2019-04-26.
//

#include "HKAccessory.h"


HKAccessory::HKAccessory(HKAccessoryCategory category) : id(0), category(category) {

}

void HKAccessory::addInfoService(const String& accName, const String& manufacturerName, const String& modelName, const String& serialNumber) {
    auto *infoService = new HKService(HKServiceType::ServiceAccessoryInfo);
    addService(infoService);

    char *convName = (char *) malloc(accName.length()+1);
    strcpy(convName, accName.c_str());
    Serial.println("AccName: " + String(convName));
    HKValue nameValue{};
    nameValue.Format = FormatString;
    nameValue.StringValue = convName;
    HKCharacteristic *nameChar = new HKCharacteristic(CharacteristicServiceName, nameValue, PermissionPairedRead, "Name");
    infoService->addCharacteristic(nameChar);

    char *convManufacturer = (char *) malloc(manufacturerName.length()+1);
    strcpy(convManufacturer, manufacturerName.c_str());
    HKValue manufacturerValue{};
    manufacturerValue.Format = FormatString;
    manufacturerValue.StringValue = convManufacturer;
    HKCharacteristic *manufacturerChar = new HKCharacteristic(CharacteristicManufactuer, manufacturerValue, PermissionPairedRead, "Manufacturer");
    infoService->addCharacteristic(manufacturerChar);

    char *convModel = (char *) malloc(modelName.length()+1);
    strcpy(convModel, modelName.c_str());
    HKValue modelValue{};
    modelValue.Format = FormatString;
    modelValue.StringValue = convModel;
    HKCharacteristic *modelChar = new HKCharacteristic(CharacteristicModelName, modelValue, PermissionPairedRead, "Model");
    infoService->addCharacteristic(modelChar);

    char *convNumber = (char *) malloc(serialNumber.length()+1);
    strcpy(convNumber, serialNumber.c_str());
    HKValue serialValue{};
    serialValue.Format = FormatString;
    serialValue.StringValue = convNumber;
    HKCharacteristic *serialChar = new HKCharacteristic(CharacteristicSerialNumber, serialValue, PermissionPairedRead, "Serial Number");
    infoService->addCharacteristic(serialChar);

    HKValue identifyValue{};
    identifyValue.Format = FormatBool;
    HKCharacteristic *identifyChar = new HKCharacteristic(CharacteristicIdentify, identifyValue, PermissionPairedWrite, "Identify");
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
