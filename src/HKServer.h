//
// Created by Max Vissing on 2019-04-26.
//

#ifndef HAP_SERVER_HKSERVER_H
#define HAP_SERVER_HKSERVER_H

#define PORT 5556

#define MAX_CLIENTS 16

#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <SHA512.h>
#include <Ed25519.h>
#include <srp.h>
#include <tweetnacl.h>
#include "HKTLV.h"
#include "HomeKit.h"

class HomeKit;

class HKServer {
public:
    explicit HKServer(HomeKit *homeKit);
    void setup();
    void update();

private:
    struct VerifyContext {
        byte accessorySecretKey[32];
        byte sharedKey[32];
        byte accessoryPublicKey[32];
        byte devicePublicKey[32];
        byte sessionKey[32];
    };

    class HKClient {
    public:
        explicit HKClient(WiFiClient client) : client(client), verifyContext(), encrypted(false), countReads(0), countWrites(0), pairingId(0), permission(0) {};
    public:
        WiFiClient client;
        VerifyContext verifyContext;
        bool encrypted;
        byte readKey[32];
        int countReads;
        byte writeKey[32];
        int countWrites;
        int pairingId;
        byte permission;
    };
private:
    int setupMDNS();

    bool received(HKClient *client);
    size_t decryptedSize(size_t payloadSize);
    byte *decrypt(HKClient *client);

    static void hkdf(byte *target, byte *ikm, uint8_t ikmLength, byte *salt, uint8_t saltLength, byte *info, uint8_t infoLength);
    static void sendTLVResponse(std::vector<HKTLV *> &message, HKClient *client);
    void sendTLVError(byte state, TLVError error, HKClient *client);

    void onPairSetup(const std::vector<byte> &body, HKClient *client);
    void onPairVerify(const std::vector<byte> &body, HKClient *client);
private:
    HomeKit *hk;

    WiFiServer *server;
    esp8266::MDNSImplementation::MDNSResponder::hMDNSService mdnsService;

    std::vector<HKClient *> clients;
    bool pairing;
};


#endif //HAP_SERVER_HKSERVER_H
