//
// Created by Max Vissing on 2019-06-07.
//

#ifndef LED_HAP_ESP8266_JSON_H
#define LED_HAP_ESP8266_JSON_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

#define JSON_MAX_DEPTH 30
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

enum JSONState {
    JSONStateStart = 1,
    JSONStateEnd,
    JSONStateObject,
    JSONStateObjectKey,
    JSONStateObjectValue,
    JSONStateArray,
    JSONStateArrayItem,
    JSONStateError
};

enum JSONNesting {
    JSONNestingObject,
    JSONNestingArray
};

class JSON {
public:
    JSON(size_t bufferSize, std::function<void(uint8_t *buffer, size_t size, void *context)> onFlush, void *context);
    virtual ~JSON();
    void flush();
    void startObject();
    void endObject();
    void startArray();
    void endArray();

    void set(long long value);
    void set(float value);
    void set(const char *value);
    void set(const String& value);
    void set(bool value);
    void set();

private:
    void write(const char *format, ...);
private:
    uint8_t *buffer;
    size_t size;
    size_t pos;

    JSONState state;
    uint8_t nestingId;
    JSONNesting nesting[JSON_MAX_DEPTH];

    std::function<void(uint8_t *buffer, size_t size, void *context)> onFlush;
    void *context;
};


#endif //LED_HAP_ESP8266_JSON_H
