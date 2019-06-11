//
// Created by Max Vissing on 2019-04-26.
//

#ifndef HAP_SERVER_HKSERVICE_H
#define HAP_SERVER_HKSERVICE_H

#include <Arduino.h>
#include <JSON.h>
#include "HKCharacteristic.h"

enum HKServiceType {
    ServiceAccessoryInfo      = 0x3E,
    ServiceCamera             = 0x3F,
    ServiceFan                = 0x40,
    ServiceGarageDoorOpener   = 0x41,
    ServiceLightBulb          = 0x43,
    ServiceLockManagement     = 0x44,
    ServiceLockMechanism      = 0x45,
    ServiceMicrophone         = 0x46,
    ServiceOutlet             = 0x47,
    ServiceSpeaker            = 0x48,
    ServiceSwitch             = 0x49,
    ServiceThermostat         = 0x4A,

    ServiceAlarmSystem        = 0x7E,
    ServiceBridgingState      = 0x62,
    ServiceCarbonMonoxideSensor = 0x7F,
    ServiceContactSensor      = 0x80,
    ServiceDoor               = 0x81,
    ServiceHumiditySensor     = 0x82,
    ServiceLeakSensor         = 0x83,
    ServiceLightSensor        = 0x84,
    ServiceMotionSensor       = 0x85,
    ServiceOccupancySensor    = 0x86,
    ServiceSmokeSensor        = 0x87,
    ServiceProgrammableSwitch_stateful = 0x88,
    ServiceProgrammableSwitch_stateless = 0x89,
    ServiceTemperatureSensor  = 0x8A,
    ServiceWindow             = 0x8B,
    ServiceWindowCover        = 0x8C,
    ServiceAirQualitySensor   = 0x8C,
    ServiceSecurityAlarm      = 0x8E,
    ServiceCharging           = 0x8F,

    ServiceBattery            = 0x96,
    ServiceCarbonDioxideSensor= 0x97,
};

class HKService {
public:
    explicit HKService(HKServiceType type);
    void setupCharacteristics(unsigned int &iid);

    HKCharacteristic *getCharacteristic(HKCharacteristicType characteristicType);
    std::vector<HKCharacteristic *> getCharacteristics();

    unsigned int getId() const;
    HKServiceType getServiceType() const;
    bool isHidden();
    bool isPrimary();
    std::vector<HKService *> getLinkedServices();

    HKCharacteristic *findCharacteristic(unsigned int id);

    void serializeToJSON(JSON &json, HKValue *value);

    void addCharacteristic(HKCharacteristic *characteristic);
    void setId(unsigned int id);

    void setPrimary(bool primary);

private:
    unsigned int id;
    HKServiceType serviceType;
    bool hidden;
    bool primary;
    std::vector<HKService *> linked;
    std::vector<HKCharacteristic *> characteristics;
};


#endif //HAP_SERVER_HKSERVICE_H
