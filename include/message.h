#pragma once

#include <Arduino.h>


// ============================================================
// TYPE DE MESSAGE
// ============================================================

enum class MessageType {
    COMMAND,
    EVENT,
    STATE,
    HEARTBEAT,
    ACK
};


// ============================================================
// STATUT TRANSPORT
// ============================================================

enum class MessageStatus {
    PENDING,
    SENT,
    DELIVERED,
    FAILED
};


// ============================================================
// STATUT D'EXECUTION
// ============================================================

enum class ExecutionStatus {
    NOT_EXECUTED,
    EXECUTED,
    PARTIAL,
    FAILED
};


// ============================================================
// MESSAGE
// ============================================================

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

    ExecutionStatus executionStatus;
};