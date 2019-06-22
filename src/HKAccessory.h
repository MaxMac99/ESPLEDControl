//
// Created by Max Vissing on 2019-04-26.
//

#ifndef HAP_SERVER_HKACCESSORY_H
#define HAP_SERVER_HKACCESSORY_H

#include <Arduino.h>
#include <JSON.h>
#include "HKClient.h"
#include "HKDefinitions.h"
#include "HKService.h"
#include "HKCharacteristic.h"

class HKService;
class HKCharacteristic;
class HKClient;


class HKAccessory {
public:
    explicit HKAccessory(HKAccessoryCategory category=HKAccessoryOther);

    virtual void identify();
    virtual void run();

    void addInfoService(const String& accName, const String& manufacturerName, const String& modelName, const String& serialNumber, const String &firmwareRevision);
    void addService(HKService *service);
    void setup();

    void clearCallbackEvents(HKClient *client);
    HKService *getService(HKServiceType serviceType);
    void serializeToJSON(JSON &json, HKValue *value, HKClient *client = nullptr);
    unsigned int getId() const;
    HKAccessoryCategory getCategory() const;
    HKCharacteristic *findCharacteristic(unsigned int iid);

    void setId(unsigned int id);
private:
    unsigned int id;
    HKAccessoryCategory category;
    std::vector<HKService *> services;
};


#endif //HAP_SERVER_HKACCESSORY_H
