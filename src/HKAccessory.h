//
// Created by Max Vissing on 2019-04-26.
//

#ifndef HAP_SERVER_HKACCESSORY_H
#define HAP_SERVER_HKACCESSORY_H

#include <Arduino.h>
#include "HKService.h"

enum HKAccessoryCategory {
    AccessoryOther               = 1,
    AccessoryBridge              = 2,
    AccessoryFan                 = 3,
    AccessoryGarage              = 4,
    AccessoryLightbulb           = 5,
    AccessoryDooLock             = 6,
    AccessoryOutlet              = 7,
    AccessorySwitch              = 8,
    AccessoryThermostat          = 9,
    AccessorySensor              = 10,
    AccessorySecuritySystem      = 11,
    AccessoryDoor                = 12,
    AccessoryWindow              = 13,
    AccessoryWindowCovering      = 14,
    AccessoryProgrammableSwitch  = 15,
    AccessoryRangeExtender       = 16,
    AccessoryIPCamera            = 17,
    AccessoryVideoDoorBell       = 18,
    AccessoryAirPurifier         = 19,
    AccessoryHeater              = 20,
    AccessoryAirConditioner      = 21,
    AccessoryHumidifier          = 22,
    AccessoryDehumidifier        = 23,
    AccessoryAppleTV             = 24,
    AccessorySpeaker             = 26,
    AccessoryAirport             = 27,
    AccessorySprinkler           = 28,
    AccessoryFaucet              = 29,
    AccessoryShowerHead          = 30,
    AccessoryTelevision          = 31,
    AccessoryTargetController    = 32
};

class HKAccessory {
public:
    explicit HKAccessory(HKAccessoryCategory category=AccessoryOther);

    void addInfoService(const String& accName, const String& manufacturerName, const String& modelName, const String& serialNumber);
    void addService(HKService *service);
    virtual void identify();
    void setupServices();

    HKService *getService(HKServiceType serviceType);
    unsigned int getId() const;
    HKAccessoryCategory getCategory() const;

    void setId(unsigned int id);
private:
    unsigned int id;

    HKAccessoryCategory category;
    std::vector<HKService *> services;
    //int config_number;
};


#endif //HAP_SERVER_HKACCESSORY_H
