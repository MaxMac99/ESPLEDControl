#include <utility>

//
// Created by Max Vissing on 2019-04-26.
//

#include "HKCharacteristic.h"

HKCharacteristic::HKCharacteristic(HKCharacteristicType type, const HKValue &value, HKPermission permissions,
                                   String description, HKUnit unit) : id(0), type(type), value(value), permissions(permissions), description(std::move(description)), unit(unit), minValue(nullptr), maxValue(nullptr), minStep(nullptr), maxLen(nullptr), maxDataLen(nullptr), validValues(), getter(nullptr), setter(nullptr) {

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
}

void HKCharacteristic::setSetter(const std::function<void(const HKValue)> &setter) {
    HKCharacteristic::setter = setter;
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
