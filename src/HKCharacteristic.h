//
// Created by Max Vissing on 2019-04-26.
//

#ifndef HAP_SERVER_HKCHARACTERISTIC_H
#define HAP_SERVER_HKCHARACTERISTIC_H

#include <Arduino.h>

enum HKCharacteristicType {
    CharacteristicAdminOnlyAccess = 0x1,
    AudioChannels = 0x2,
    CharacteristicAudioCodexName = 0x3,
    CharacteristicAudioCodexParameter = 0x4,
    CharacteristicAudioFeedback = 0x5,
    CharacteristicAudioPropAttr = 0x6,
    CharacteristicAudioValAttr = 0x7,
    CharacteristicBrightness = 0x8,
    CharacteristicCameraNightVision = 0x9,
    CharacteristicCameraPan = 0xA,
    CharacteristicCameraTilt = 0xB,
    CharacteristicCameraZoom = 0xC,
    CharacteristicCoolingThreshold = 0xD,
    CharacteristicCurrentDoorState = 0xE,
    CharacteristicCurrentHeatCoolMode = 0xF,
    CharacteristicCurrentHumidity = 0x10,
    CharacteristicCurrentTemperature = 0x11,
    CharacteristicHeatingThreshold = 0x12,
    CharacteristicHue = 0x13,
    CharacteristicIdentify = 0x14,
    CharacteristicInputVolume = 0x15,
    CharacteristicIpCameraStart = 0x16,
    CharacteristicIpCameraStop = 0x17,
    CharacteristicLockControlPoint = 0x19,
    CharacteristicLockAutoTimeout = 0x1A,
    CharacteristicLockLastAction = 0x1C,
    CharacteristicLockCurrentState = 0x1D,
    CharacteristicLockTargetState = 0x1E,
    CharacteristicLogs = 0x1F,
    CharacteristicManufactuer = 0x20,
    CharacteristicModelName = 0x21,
    CharacteristicMotionDetect = 0x22,
    CharacteristicServiceName = 0x23,
    CharacteristicObstruction = 0x24,
    CharacteristicOn = 0x25,
    CharacteristicOutletInUse = 0x26,
    CharacteristicOutputVolume = 0x27,
    CharacteristicRotationDirection = 0x28,
    CharacteristicRotationSpeed = 0x29,
    CharacteristicRtcpExtProp = 0x2A,
    CharacteristicRtcpVideoPayload = 0x2B,
    CharacteristicRtcpAudioPayload = 0x2C,
    CharacteristicRtcpAudioClock = 0x2D,
    CharacteristicRtcpProtocol = 0x2E,
    CharacteristicSaturation = 0x2F,
    CharacteristicSerialNumber = 0x30,
    CharacteristicSrtpCyptoSuite = 0x31,
    CharacteristicTargetDoorState = 0x32,
    CharacteristicTargetHeatCoolMode = 0x33,
    CharacteristicTargetHumidity = 0x34,
    CharacteristicTargetTemperature = 0x35,
    CharacteristicTemperatureUnit = 0x36,
    CharacteristicVersion = 0x37,
    CharacteristicVideoCodexName = 0x38,
    CharacteristicVideoCodexPara = 0x39,
    CharacteristicVideoMirror = 0x3A,
    CharacteristicVideoPropAttr = 0x3B,
    CharacteristicVideoRotation = 0x3C,
    CharacteristicVideoValAttr = 0x3D,

#pragma - The following is only default by the device after iOS 9

    CharacteristicFirmwareRevision = 0x52,
    CharacteristicHardwareRevision = 0x53,
    CharacteristicSoftwareRevision = 0x54,

    CharacteristicReachable = 0x63,

    CharacteristicAirParticulateDensity = 0x64,
    CharacteristicAirParticulateSize = 0x65,
    CharacteristicAirQuality = 0x95,
    CharacteristicCarbonDioxideDetected = 0x92,
    CharacteristicCarbonMonoxideDetected = 0x69,
    CharacteristicCarbonDioxideLevel = 0x93,
    CharacteristicCarbonMonoxideLevel = 0x90,
    CharacteristicCarbonDioxidePeakLevel = 0x94,
    CharacteristicCarbonMonoxidePeakLevel = 0x91,
    CharacteristicSmokeDetected = 0x76,

    CharacteristicAlarmCurrentState = 0x66,
    CharacteristicAlarmTargetState = 0x67,
    CharacteristicBatteryLevel = 0x68,
    CharacteristicContactSensorState = 0x6A,
    CharacteristicHoldPosition = 0x6F,
    CharacteristicLeakDetected = 0x70,
    CharacteristicOccupancyDetected = 0x71,

    CharacteristicCurrentAmbientLightLevel = 0x6B,
    CharacteristicCurrentHorizontalTiltAngle = 0x6C,
    CharacteristicTargetHorizontalTiltAngle = 0x7B,
    CharacteristicCurrentPosition = 0x6D,
    CharacteristicTargetPosition = 0x7C,
    CharacteristicCurrentVerticalTiltAngle = 0x6E,
    CharacteristicTargetVerticalTiltAngle = 0x7D,

    CharacteristicPositionState = 0x72,
    CharacteristicProgrammableSwitchEvent = 0x73,
    CharacteristicProgrammableSwitchOutputState = 0x74,

    CharacteristicSensorActive = 0x75,
    CharacteristicSensorFault = 0x77,
    CharacteristicSensorJammed = 0x78,
    CharacteristicSensorLowBattery = 0x79,
    CharacteristicSensorTampered = 0x7A,
    CharacteristicSensorChargingState = 0x8F,
};

enum HKFormat {
    FormatBool,
    FormatUInt8,
    FormatUInt16,
    FormatUInt32,
    FormatUInt64,
    FormatInt,
    FormatFloat,
    FormatString,
    FormatTLV,
    FormatData
};

enum HKUnit {
    UnitNone,
    Celsius,
    UnitPercentage,
    UnitArcdegrees,
    UnitLux,
    UnitSeconds
};

enum HKPermission {
    PermissionPairedRead = 1,
    PermissionPairedWrite = 2,
    PermissionNotify = 4,
    PermissionAdditionalAuthorization = 8,
    PermissionTimedWrite = 16,
    PermissionHidden = 32
};

struct HKValue {
    bool IsNull : 1;
    bool IsStatic : 1;
    HKFormat Format : 6;
    union {
        bool BoolValue;
        int IntValue;
        float FloatValue;
        char *StringValue;
        //TLVValues *tlvValues;
        // Data
    };
};

struct HKValidValues {
    int count;
    uint8_t *values;
};

class HKCharacteristic {
public:
    HKCharacteristic(HKCharacteristicType type, const HKValue &value, HKPermission permissions,
                     String description, HKUnit unit=UnitNone);

    HKCharacteristicType getType() const;
    const HKValue &getValue() const;
    unsigned int getId() const;

    void setMinValue(float *minValue);
    void setMaxValue(float *maxValue);
    void setMinStep(float *minStep);
    void setMaxLen(int *maxLen);
    void setMaxDataLen(int *maxDataLen);
    void setValidValues(const HKValidValues &validValues);
    void setGetter(const std::function<HKValue()> &getter);
    void setSetter(const std::function<void(const HKValue)> &setter);
    void setId(unsigned int id);

private:
    unsigned int id;

    HKCharacteristicType type;
    HKValue value;
    HKPermission permissions;
    String description;
    HKUnit unit;

    float *minValue;
    float *maxValue;
    float *minStep;
    int *maxLen;
    int *maxDataLen;

    HKValidValues validValues;

    std::function<HKValue()> getter;
    std::function<void(const HKValue)> setter;
};


#endif //HAP_SERVER_HKCHARACTERISTIC_H
