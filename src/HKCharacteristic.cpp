#include <utility>

//
// Created by Max Vissing on 2019-04-26.
//

#include "HKCharacteristic.h"
#include "HKClient.h"

HKCharacteristic::HKCharacteristic(HKCharacteristicType type, const HKValue &value, uint8_t permissions,
                                   String description, HKFormat format, HKUnit unit) : id(0), type(type), value(value), permissions(permissions), description(std::move(description)), unit(unit), format(format), minValue(nullptr), maxValue(nullptr), minStep(nullptr), maxLen(nullptr), maxDataLen(nullptr), validValues(), validValuesRanges(), getter(nullptr), setter(nullptr), callback(nullptr), context(nullptr) {

}

void HKCharacteristic::setMinValue(float *minValue) {
    HKCharacteristic::minValue = minValue;
}

void HKCharacteristic::setMaxValue(float *maxValue) {
    HKCharacteristic::maxValue = maxValue;
}

void HKCharacteristic::setMinStep(float *minStep) {
    HKCharacteristic::minStep = minStep;
}

void HKCharacteristic::setMaxLen(int *maxLen) {
    HKCharacteristic::maxLen = maxLen;
}

void HKCharacteristic::setMaxDataLen(int *maxDataLen) {
    HKCharacteristic::maxDataLen = maxDataLen;
}

void HKCharacteristic::setValidValues(const HKValidValues &validValues) {
    HKCharacteristic::validValues = validValues;
}

void HKCharacteristic::setGetter(const std::function<HKValue()> &getter) {
    HKCharacteristic::getter = getter;
    getterEx = std::bind(&HKCharacteristic::exOldGetter, std::placeholders::_1);
}

void HKCharacteristic::setSetter(const std::function<void(const HKValue)> &setter) {
    HKCharacteristic::setter = setter;
    setterEx = std::bind(&HKCharacteristic::exOldSetter, std::placeholders::_1, std::placeholders::_2);
}

HKCharacteristicType HKCharacteristic::getType() const {
    return type;
}

const HKValue &HKCharacteristic::getValue() const {
    return value;
}

unsigned int HKCharacteristic::getId() const {
    return id;
}

void HKCharacteristic::setId(unsigned int id) {
    HKCharacteristic::id = id;
}

uint8_t HKCharacteristic::getPermissions() const {
    return permissions;
}

const String &HKCharacteristic::getDescription() const {
    return description;
}

HKFormat HKCharacteristic::getFormat() const {
    return format;
}

HKUnit HKCharacteristic::getUnit() const {
    return unit;
}

float *HKCharacteristic::getMinValue() const {
    return minValue;
}

float *HKCharacteristic::getMaxValue() const {
    return maxValue;
}

float *HKCharacteristic::getMinStep() const {
    return minStep;
}

int *HKCharacteristic::getMaxLen() const {
    return maxLen;
}

int *HKCharacteristic::getMaxDataLen() const {
    return maxDataLen;
}

const HKValidValues &HKCharacteristic::getValidValues() const {
    return validValues;
}

const HKValidValuesRanges &HKCharacteristic::getValidValuesRanges() const {
    return validValuesRanges;
}

void HKCharacteristic::serializeToJSON(JSON &json, HKValue *jsonValue, unsigned int jsonFormatOptions) {
    json.setString("iid");
    json.setInt(id);

    if (jsonFormatOptions & HKCharacteristicFormatType) {
        json.setString("type");
        String typeConv = String(type, HEX);
        typeConv.toUpperCase();
        json.setString(typeConv.c_str());
    }

    if (jsonFormatOptions & HKCharacteristicFormatPerms) {
        json.setString("perms");
        json.startArray();
        if (permissions & PermissionPairedRead) {
            json.setString("pr");
        }
        if (permissions & PermissionPairedWrite) {
            json.setString("pw");
        }
        if (permissions & PermissionNotify) {
            json.setString("ev");
        }
        if (permissions & PermissionAdditionalAuthorization) {
            json.setString("aa");
        }
        if (permissions & PermissionTimedWrite) {
            json.setString("tw");
        }
        if (permissions & PermissionHidden) {
            json.setString("hd");
        }
        json.endArray();
    }

    if ((jsonFormatOptions & HKCharacteristicFormatEvents) && (permissions & PermissionNotify)) {
        json.setString("ev");
        // TODO: json.setBool(hasNotifyCallback(HKCharacteristic::notifyClient, this));
        json.setBool(false);
    }

    if (jsonFormatOptions & HKCharacteristicFormatMeta) {
        json.setString("description");
        json.setString(description.c_str());

        json.setString("format");
        switch (format) {
            case FormatBool:
                json.setString("bool");
                break;
            case FormatUInt8:
                json.setString("uint8");
                break;
            case FormatUInt16:
                json.setString("uint16");
                break;
            case FormatUInt32:
                json.setString("uint32");
                break;
            case FormatUInt64:
                json.setString("uint64");
                break;
            case FormatInt:
                json.setString("int");
                break;
            case FormatFloat:
                json.setString("float");
                break;
            case FormatString:
                json.setString("string");
                break;
            case FormatTLV:
                json.setString("tlv8");
                break;
            case FormatData:
                json.setString("data");
                break;
        }

        switch (unit) {
            case UnitNone:
                break;
            case Celsius:
                json.setString("unit");
                json.setString("celsius");
                break;
            case UnitPercentage:
                json.setString("unit");
                json.setString("percentage");
                break;
            case UnitArcdegrees:
                json.setString("unit");
                json.setString("arcdegrees");
                break;
            case UnitLux:
                json.setString("unit");
                json.setString("lux");
                break;
            case UnitSeconds:
                json.setString("unit");
                json.setString("seconds");
                break;
        }

        if (minValue) {
            json.setString("minValue");
            json.setFloat(*minValue);
        }

        if (maxValue) {
            json.setString("maxValue");
            json.setFloat(*maxValue);
        }

        if (minStep) {
            json.setString("minStep");
            json.setFloat(*minStep);
        }

        if (maxLen) {
            json.setString("maxLen");
            json.setFloat(*maxLen);
        }

        if (maxDataLen) {
            json.setString("maxDataLen");
            json.setFloat(*maxDataLen);
        }

        if (validValues.count) {
            json.setString("valid-values");
            json.startArray();

            for (int i = 0; i < validValues.count; i++) {
                json.setInt(validValues.values[i]);
            }
            json.endArray();
        }

        if (validValuesRanges.count) {
            json.setString("valid-values-range");
            json.startArray();

            for (int i = 0; i < validValuesRanges.count; i++) {
                json.startArray();

                json.setInt(validValuesRanges.ranges[i].start);
                json.setInt(validValuesRanges.ranges[i].end);

                json.endArray();
            }
            json.endArray();
        }
    }

    if (permissions & PermissionPairedRead) {
        HKValue v = jsonValue ? *jsonValue : getterEx ? getterEx(this) : value;

        if (v.isNull) {
            json.setString("value");
            json.setNull();
        } else if (v.format != format) {
            Serial.println("Characteristic value format is different from characteristic format");
            Serial.println("format: " + String(format) + " v.format: " + String(v.format));
        } else {
            switch (v.format) {
                case FormatBool:
                    json.setString("value");
                    json.setBool(v.boolValue);
                    break;
                case FormatUInt8:
                case FormatUInt16:
                case FormatUInt32:
                case FormatUInt64:
                case FormatInt:
                    json.setString("value");
                    json.setInt(v.intValue);
                    break;
                case FormatFloat:
                    json.setString("value");
                    json.setFloat(v.floatValue);
                    break;
                case FormatString:
                    json.setString("value");
                    json.setString(v.stringValue);
                    break;
                case FormatTLV:
                    break;
                case FormatData:
                    break;
                default:
                    break;
            }
        }
    }
}

void HKCharacteristic::setValue(JsonVariant jsonValue) {
    if (!(permissions & PermissionPairedWrite)) {
        Serial.println("Failed to update: no write permission");
        return;
    }

    switch (format) {
        case FormatBool: {
            bool result;
            if (jsonValue.is<bool>()) {
                result = jsonValue.as<bool>();
            } else if (jsonValue.is<int>()) {
                result = jsonValue.as<int>() == 1;
            } else {
                Serial.println("Failed to update: Json is not of type bool");
                return;
            }

            Serial.println("Update Characteristic with bool: " + String(result));

            if (setterEx) {
                HKValue newValue = HKValue::setBool(result);
                setterEx(this, newValue);
            } else {
                value.boolValue = result;
            }
            break;
        }
        case FormatUInt8:
        case FormatUInt16:
        case FormatUInt32:
        case FormatUInt64:
        case FormatInt: {
            uint64_t result;
            if (jsonValue.is<uint64_t>() || jsonValue.is<bool>()) {
                result = jsonValue.as<uint64_t>();
            } else {
                Serial.println("Failed to update: Json is not of type int");
                return;
            }

            uint64_t checkMinValue = 0;
            uint64_t checkMaxValue = 0;
            switch (format) {
                case FormatUInt8: {
                    checkMinValue = 0;
                    checkMaxValue = 0xFF;
                    break;
                }
                case FormatUInt16:{
                    checkMinValue = 0;
                    checkMaxValue = 0xFFFF;
                    break;
                }
                case FormatUInt32: {
                    checkMinValue = 0;
                    checkMaxValue = 0xFFFFFFFF;
                    break;
                }
                case FormatUInt64: {
                    checkMinValue = 0;
                    checkMaxValue = 0xFFFFFFFFFFFFFFFF;
                    break;
                }
                case FormatInt: {
                    checkMinValue = -2147483648;
                    checkMaxValue = 2147483647;
                    break;
                }
                default:
                    break;
            }

            if (minValue) {
                checkMinValue = std::llrintf(*minValue);
            }
            if (maxValue) {
                checkMaxValue = std::llrintf(*maxValue);
            }

            if (result < checkMinValue || result > checkMaxValue) {
                Serial.println("Failed to update: int is not in range");
                return;
            }

            if (validValues.count) {
                bool matches = false;
                for (int i = 0; i < validValues.count; i++) {
                    if (result == validValues.values[i]) {
                        matches = true;
                        break;
                    }
                }

                if (!matches) {
                    Serial.println("Failed to update: int is not one of valid values");
                    return;
                }
            }

            if (validValuesRanges.count) {
                bool matches = false;
                for (int i = 0; i < validValuesRanges.count; i++) {
                    if (result >= validValuesRanges.ranges[i].start && result <= validValuesRanges.ranges[i].end) {
                        matches = true;
                    }
                }

                if (!matches) {
                    Serial.println("Failed to update: int is not one of valid values range");
                    return;
                }
            }

            Serial.println("Update Characteristic with int: " + String((uint32_t) result));

            if (setterEx) {
                HKValue newValue = HKValue::setInt(result);
                setterEx(this, newValue);
            } else {
                value.intValue = result;
            }
            break;
        }
        case FormatFloat: {
            float result;
            if (jsonValue.is<float>()) {
                result = jsonValue.as<float>();
            } else {
                Serial.println("Failed to update: Json is not of type float");
                return;
            }

            if ((minValue && result < *minValue) || (maxValue && result > *maxValue)) {
                Serial.println("Failed to update: float is not in range");
                return;
            }

            Serial.println("Update Characteristic with float: " + String(result));

            if (setterEx) {
                HKValue newValue = HKValue::setFloat(result);
                setterEx(this, newValue);
            } else {
                value.floatValue = result;
            }
            break;
        }
        case FormatString: {
            const char *result;
            if (jsonValue.is<char *>()) {
                result = jsonValue.as<char *>();
            } else {
                Serial.println("Failed to update: Json is not of type string");
                return;
            }

            int checkMaxLen = maxLen ? *maxLen : 64;
            if (strlen(result) > checkMaxLen) {
                Serial.println("Failed to update: String is too long");
                return;
            }

            Serial.println("Update Characteristic with string: " + String(result));

            if (setterEx) {
                HKValue newValue = HKValue::setString(result);
                setterEx(this, newValue);
            } else {
                value.stringValue = result;
            }
            break;
        }
        case FormatTLV: {
            Serial.println("TLV not supported yet");
            break;
        }
        case FormatData: {
            Serial.println("Data not supported yet");
            break;
        }
    }
}

HKValue HKCharacteristic::exOldGetter(const HKCharacteristic *characteristic) {
    return characteristic->getter();
}

void HKCharacteristic::exOldSetter(const HKCharacteristic *characteristic, HKValue value) {
    characteristic->setter(value);
}

