//
// Created by Max Vissing on 2019-04-26.
//

#ifndef HAP_SERVER_HKCHARACTERISTIC_H
#define HAP_SERVER_HKCHARACTERISTIC_H

#include <Arduino.h>
#include <JSON.h>
#include <ArduinoJson.h>

enum HKCharacteristicType {
    HKCharacteristicAdminOnlyAccess = 0x1,
    HKCharacteristicAudioChannels = 0x2,
    HKCharacteristicAudioCodexName = 0x3,
    HKCharacteristicAudioCodexParameter = 0x4,
    HKCharacteristicAudioFeedback = 0x5,
    HKCharacteristicAudioPropAttr = 0x6,
    HKCharacteristicAudioValAttr = 0x7,
    HKCharacteristicBrightness = 0x8,
    HKCharacteristicCameraNightVision = 0x9,
    HKCharacteristicCameraPan = 0xA,
    HKCharacteristicCameraTilt = 0xB,
    HKCharacteristicCameraZoom = 0xC,
    HKCharacteristicCoolingThreshold = 0xD,
    HKCharacteristicCurrentDoorState = 0xE,
    HKCharacteristicCurrentHeatCoolMode = 0xF,
    HKCharacteristicCurrentHumidity = 0x10,
    HKCharacteristicCurrentTemperature = 0x11,
    HKCharacteristicHeatingThreshold = 0x12,
    HKCharacteristicHue = 0x13,
    HKCharacteristicIdentify = 0x14,
    HKCharacteristicInputVolume = 0x15,
    HKCharacteristicIpCameraStart = 0x16,
    HKCharacteristicIpCameraStop = 0x17,
    HKCharacteristicLockControlPoint = 0x19,
    HKCharacteristicLockAutoTimeout = 0x1A,
    HKCharacteristicLockLastAction = 0x1C,
    HKCharacteristicLockCurrentState = 0x1D,
    HKCharacteristicLockTargetState = 0x1E,
    HKCharacteristicLogs = 0x1F,
    HKCharacteristicManufactuer = 0x20,
    HKCharacteristicModelName = 0x21,
    HKCharacteristicMotionDetect = 0x22,
    HKCharacteristicServiceName = 0x23,
    HKCharacteristicObstruction = 0x24,
    HKCharacteristicOn = 0x25,
    HKCharacteristicOutletInUse = 0x26,
    HKCharacteristicOutputVolume = 0x27,
    HKCharacteristicRotationDirection = 0x28,
    HKCharacteristicRotationSpeed = 0x29,
    HKCharacteristicRtcpExtProp = 0x2A,
    HKCharacteristicRtcpVideoPayload = 0x2B,
    HKCharacteristicRtcpAudioPayload = 0x2C,
    HKCharacteristicRtcpAudioClock = 0x2D,
    HKCharacteristicRtcpProtocol = 0x2E,
    HKCharacteristicSaturation = 0x2F,
    HKCharacteristicSerialNumber = 0x30,
    HKCharacteristicSrtpCyptoSuite = 0x31,
    HKCharacteristicTargetDoorState = 0x32,
    HKCharacteristicTargetHeatCoolMode = 0x33,
    HKCharacteristicTargetHumidity = 0x34,
    HKCharacteristicTargetTemperature = 0x35,
    HKCharacteristicTemperatureUnit = 0x36,
    HKCharacteristicVersion = 0x37,
    HKCharacteristicVideoCodexName = 0x38,
    HKCharacteristicVideoCodexPara = 0x39,
    HKCharacteristicVideoMirror = 0x3A,
    HKCharacteristicVideoPropAttr = 0x3B,
    HKCharacteristicVideoRotation = 0x3C,
    HKCharacteristicVideoValAttr = 0x3D,

#pragma - The following is only default by the device after iOS 9

    HKCharacteristicFirmwareRevision = 0x52,
    HKCharacteristicHardwareRevision = 0x53,
    HKCharacteristicSoftwareRevision = 0x54,

    HKCharacteristicReachable = 0x63,

    HKCharacteristicAirParticulateDensity = 0x64,
    HKCharacteristicAirParticulateSize = 0x65,
    HKCharacteristicAirQuality = 0x95,
    HKCharacteristicCarbonDioxideDetected = 0x92,
    HKCharacteristicCarbonMonoxideDetected = 0x69,
    HKCharacteristicCarbonDioxideLevel = 0x93,
    HKCharacteristicCarbonMonoxideLevel = 0x90,
    HKCharacteristicCarbonDioxidePeakLevel = 0x94,
    HKCharacteristicCarbonMonoxidePeakLevel = 0x91,
    HKCharacteristicSmokeDetected = 0x76,

    HKCharacteristicAlarmCurrentState = 0x66,
    HKCharacteristicAlarmTargetState = 0x67,
    HKCharacteristicBatteryLevel = 0x68,
    HKCharacteristicContactSensorState = 0x6A,
    HKCharacteristicHoldPosition = 0x6F,
    HKCharacteristicLeakDetected = 0x70,
    HKCharacteristicOccupancyDetected = 0x71,

    HKCharacteristicCurrentAmbientLightLevel = 0x6B,
    HKCharacteristicCurrentHorizontalTiltAngle = 0x6C,
    HKCharacteristicTargetHorizontalTiltAngle = 0x7B,
    HKCharacteristicCurrentPosition = 0x6D,
    HKCharacteristicTargetPosition = 0x7C,
    HKCharacteristicCurrentVerticalTiltAngle = 0x6E,
    HKCharacteristicTargetVerticalTiltAngle = 0x7D,

    HKCharacteristicPositionState = 0x72,
    HKCharacteristicProgrammableSwitchEvent = 0x73,
    HKCharacteristicProgrammableSwitchOutputState = 0x74,

    HKCharacteristicSensorActive = 0x75,
    HKCharacteristicSensorFault = 0x77,
    HKCharacteristicSensorJammed = 0x78,
    HKCharacteristicSensorLowBattery = 0x79,
    HKCharacteristicSensorTampered = 0x7A,
    HKCharacteristicSensorChargingState = 0x8F,
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

enum HKCharacteristicFormat {
    HKCharacteristicFormatType = 1,
    HKCharacteristicFormatMeta = 2,
    HKCharacteristicFormatPerms = 4,
    HKCharacteristicFormatEvents = 8
};

struct HKValue {
    bool isNull : 1;
    bool isStatic : 1;
    HKFormat format : 6;
    union {
        bool boolValue;
        int intValue;
        float floatValue;
        const char *stringValue;
        //TLVValues *tlvValues;
        // Data
    };
    static HKValue setBool(bool value) {
        HKValue item{};
        item.format = FormatBool;
        item.boolValue = value;
        return item;
    }
    static HKValue setInt(int value) {
        HKValue item{};
        item.format = FormatBool;
        item.intValue = value;
        return item;
    }
    static HKValue setFloat(float value) {
        HKValue item{};
        item.format = FormatBool;
        item.floatValue = value;
        return item;
    }
    static HKValue setString(const char *value) {
        HKValue item{};
        item.format = FormatBool;
        item.stringValue = value;
        return item;
    }
    bool operator==(const HKValue& b)
    {
        if (isNull != b.isNull) {
            return false;
        }
        if (format != b.format) {
            return false;
        }

        switch (format) {
            case FormatBool:
                return boolValue == b.boolValue;
            case FormatUInt8:
            case FormatUInt16:
            case FormatUInt32:
            case FormatUInt64:
            case FormatInt:
                return intValue == b.intValue;
            case FormatFloat:
                return floatValue == b.floatValue;
            case FormatString:
                return !strcmp(stringValue, b.stringValue);
            case FormatTLV:
                /*if (!tlvValues && !b.tlvValues) {
                    return true;
                }
                if (!tlvValues || !b.tlvValues) {
                    return false;
                }
                */
                return false;

            case FormatData:
                return false;
            default:
                return false;
        }
    }
};

struct HKValidValues {
    int count;
    uint8_t *values;
};

struct HKValidValuesRange {
    uint8_t start;
    uint8_t end;
};

struct HKValidValuesRanges {
    int count;
    HKValidValuesRange *ranges;
};

class HKCharacteristic;

typedef void (*ChangeCallbackFn)(HKCharacteristic *ch, HKValue value, void *context);

class HKCharacteristic {
public:
    HKCharacteristic(HKCharacteristicType type, const HKValue &value, uint8_t permissions,
                     String description, HKFormat format, HKUnit unit = UnitNone);

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

    void serializeToJSON(JSON &json, HKValue *jsonValue, unsigned int format = 0xF);
    void setValue(JsonVariant jsonValue);

    uint8_t getPermissions() const;
    const String &getDescription() const;
    HKUnit getUnit() const;
    HKFormat getFormat() const;
    float *getMinValue() const;
    float *getMaxValue() const;
    float *getMinStep() const;
    int *getMaxLen() const;
    int *getMaxDataLen() const;
    const HKValidValues &getValidValues() const;
    const HKValidValuesRanges &getValidValuesRanges() const;

    struct ChangeCallback {
        ChangeCallbackFn function;
        void *context;
        ChangeCallback *next;
    };
private:
    static HKValue exOldGetter(const HKCharacteristic *characteristic);
    static void exOldSetter(const HKCharacteristic *characteristic, HKValue value);
private:
    unsigned int id;

    HKCharacteristicType type;
    HKValue value;
    uint8_t permissions;
    String description;
    HKUnit unit;
    HKFormat format;

    float *minValue;
    float *maxValue;
    float *minStep;
    int *maxLen;
    int *maxDataLen;

    HKValidValues validValues;
    HKValidValuesRanges validValuesRanges;

    std::function<HKValue()> getter;
    std::function<void(const HKValue)> setter;

    ChangeCallback *callback;
    std::function<HKValue(const HKCharacteristic *)> getterEx;
    std::function<void(const HKCharacteristic *, const HKValue)> setterEx;

    void *context;
};


#endif //HAP_SERVER_HKCHARACTERISTIC_H
