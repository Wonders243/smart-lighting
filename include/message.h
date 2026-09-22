#pragma once

#include <Arduino.h>

enum class MessageType {
    COMMAND,
    EVENT,
    STATE,
    HEARTBEAT,
    ACK
};

enum class MessageStatus {
    PENDING,
    SENT,
    DELIVERED,
    FAILED
};

struct Message {
    uint32_t id;

    uint32_t sourceId;
    uint32_t destinationId;

    MessageType type;

    uint32_t timestamp;

    int32_t commandType;
    int32_t value;
    int32_t value2;

    MessageStatus status;
};