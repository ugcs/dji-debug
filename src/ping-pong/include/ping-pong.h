#pragma once
#include <dji_typedef.h>
#include <vector>

namespace C {
    const int MaxFrameLength = 512;
    const uint8_t PingMessageIndex = 0xFE;
    const uint8_t PongMessageIndex = 0x04;
    const uint8_t VSM_PROTOCOL_VERSION_MAJOR = 0;
    const uint8_t VSM_PROTOCOL_VERSION_MINOR = 3;
}

struct OnboardMessageHeader
{
    uint8_t id;
    uint8_t length;

    static const size_t size = sizeof(id) + sizeof(length);
};

struct PingPayload {
    int64_t timeStampMs;
    /**
     * Major protocol version received from VSM. Default: 0
     * @brief major
     */
    uint8_t major = 0;
    /**
     * Minor protocol version received from VSM. Default: 1
     * @brief minor
     */
    uint8_t minor = 1;
};

struct PongMessage {
    uint64_t timestamp;
    /**
     * Major protocol version on UgCS SkyHub
     * @brief major
     */
    uint8_t major;
    /**
     * Minor protocol Version on UgCS SkyHub
     * @brief minor
     */
    uint8_t minor;

    static const size_t size = sizeof(timestamp) + sizeof(major) + sizeof(minor);
};

T_DjiReturnCode onMobileDataReceived(const uint8_t *data, uint16_t len);
void receivedData(const std::vector<uint8_t>& data);
void parseIncomingData(const std::vector<uint8_t> &data);
void sendPong(uint64_t timestamp);
uint16_t calcCrc(const void* data, size_t length);
void sendDataToMSDK(const std::vector<uint8_t>& data);
void* UserDataTransmission_Task(void *arg);


