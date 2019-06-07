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
    void reply(HKClient *client, byte *message, size_t messageSize);
    static void sendEncrypted(HKClient *client, byte *message, size_t messageSize);
    static byte *receivedDecrypted(HKClient *client, size_t &decryptedSize);
    void sendTLVResponse(HKClient *client, std::vector<HKTLV *> &message);
    void sendTLVError(HKClient *client, byte state, TLVError error);

    static void hkdf(byte *target, byte *ikm, uint8_t ikmLength, byte *salt, uint8_t saltLength, byte *info, uint8_t infoLength);

    void onPairSetup(HKClient *client, const std::vector<byte> &body);
    void onPairVerify(HKClient *client, const std::vector<byte> &body);
    void onIdentify(HKClient *client);
    void onGetAccessories(HKClient *client);
    void onGetCharacteristics(HKClient *client);
    void onUpdateCharacteristics(HKClient *client, const std::vector<byte> &body);
    void onPairings(HKClient *client, const std::vector<byte> &body);
    void onReset(HKClient *client);
    void onResource(HKClient *client);
private:
    HomeKit *hk;

    WiFiServer *server;
    esp8266::MDNSImplementation::MDNSResponder::hMDNSService mdnsService;

    std::vector<HKClient *> clients;
    bool pairing;
};


#endif //HAP_SERVER_HKSERVER_H
