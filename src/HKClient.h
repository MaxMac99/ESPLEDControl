//
// Created by Max Vissing on 2019-06-07.
//

#ifndef LED_HAP_ESP8266_HKCLIENT_H
#define LED_HAP_ESP8266_HKCLIENT_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <srp.h>
#include "HKTLV.h"
#include "HKServer.h"

struct VerifyContext {
    byte accessorySecretKey[32];
    byte sharedKey[32];
    byte accessoryPublicKey[32];
    byte devicePublicKey[32];
    byte sessionKey[32];
};

class HKServer;

class HKClient : public WiFiClient {
protected:
    HKClient(HKServer *server, ClientContext* client);

public:
    ~HKClient() override;
    bool received();

    friend class HKServer;
private:
    bool readBytesWithTimeout(size_t maxLength, std::vector<byte> &data, int timeout_ms);
    byte *receivedDecrypted(size_t &decryptedSize);
    void send(byte *message, size_t messageSize);
    void sendEncrypted(byte *message, size_t messageSize);
    void sendTLVResponse(std::vector<HKTLV *> &message);
    void sendTLVError(byte state, TLVError error);

    static void hkdf(byte *target, byte *ikm, uint8_t ikmLength, byte *salt, uint8_t saltLength, byte *info, uint8_t infoLength);

    void onPairSetup(const std::vector<byte> &body);
    void onPairVerify(const std::vector<byte> &body);
    void onIdentify();
    void onGetAccessories();
    void onGetCharacteristics();
    void onUpdateCharacteristics(const std::vector<byte> &body);
    void onPairings(const std::vector<byte> &body);
    void onReset();
    void onResource();
private:
    HKServer *server;
    VerifyContext *verifyContext;
    bool encrypted;
    byte readKey[32];
    int countReads;
    byte writeKey[32];
    int countWrites;
    int pairingId;
    byte permission;
private:
    enum HTTPMethod {
        HTTP_ANY,
        HTTP_GET,
        HTTP_POST,
        HTTP_PUT,
        HTTP_PATCH,
        HTTP_DELETE,
        HTTP_OPTIONS
    };
};


#endif //LED_HAP_ESP8266_HKCLIENT_H
