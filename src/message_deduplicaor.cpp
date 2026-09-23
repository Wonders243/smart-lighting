#include <Arduino.h>

#include "message_deduplicator.h"
#include "message_manager.h"

void initMessageDeduplicator(
    MessageDeduplicator& deduplicator
) {
    deduplicator.count = 0;
    deduplicator.nextIndex = 0;

    for (
        uint8_t i = 0;
        i < MAX_PROCESSED_MESSAGES;
        i++
    ) {
        deduplicator.messages[i].messageId = 0;

        deduplicator.messages[i].executionStatus =
            ExecutionStatus::NOT_EXECUTED;

        deduplicator.messages[i].processedAt = 0;

        deduplicator.messages[i].valid = false;
    }
}

ProcessedMessage* findProcessedMessage(
    MessageDeduplicator& deduplicator,
    uint32_t messageId
) {
    for (
        uint8_t i = 0;
        i < MAX_PROCESSED_MESSAGES;
        i++
    ) {
        ProcessedMessage& message =
            deduplicator.messages[i];

        if (
            message.valid &&
            message.messageId == messageId
        ) {
            return &message;
        }
    }

    return nullptr;
}

bool isMessageProcessed(
    MessageDeduplicator& deduplicator,
    uint32_t messageId
) {
    return findProcessedMessage(
        deduplicator,
        messageId
    ) != nullptr;
}

bool registerProcessedMessage(
    MessageDeduplicator& deduplicator,
    uint32_t messageId,
    ExecutionStatus executionStatus
) {
    /*
     * Le message existe déjà.
     */
    ProcessedMessage* existing =
        findProcessedMessage(
            deduplicator,
            messageId
        );

    if (existing != nullptr) {

        existing->executionStatus =
            executionStatus;

        existing->processedAt =
            millis();

        return false;
    }

    /*
     * Si le tableau n'est pas encore plein,
     * on ajoute simplement le message.
     */
    if (
        deduplicator.count <
        MAX_PROCESSED_MESSAGES
    ) {

        ProcessedMessage& entry =
            deduplicator.messages[
                deduplicator.count
            ];

        entry.messageId = messageId;

        entry.executionStatus =
            executionStatus;

        entry.processedAt =
            millis();

        entry.valid = true;

        deduplicator.count++;

        return true;
    }

    /*
     * Tableau plein :
     *
     * on remplace l'entrée la plus ancienne
     * selon le principe d'un buffer circulaire.
     */
    ProcessedMessage& entry =
        deduplicator.messages[
            deduplicator.nextIndex
        ];

    entry.messageId = messageId;

    entry.executionStatus =
        executionStatus;

    entry.processedAt =
        millis();

    entry.valid = true;

    deduplicator.nextIndex++;

    if (
        deduplicator.nextIndex >=
        MAX_PROCESSED_MESSAGES
    ) {
        deduplicator.nextIndex = 0;
    }

    return true;
}

void printMessageDeduplicator(
    const MessageDeduplicator& deduplicator
) {
    Serial.println();

    Serial.println(
        "===== MESSAGE DEDUPLICATOR ====="
    );

    Serial.print(
        "Messages memorises : "
    );

    Serial.println(
        deduplicator.count
    );

    for (
        uint8_t i = 0;
        i < deduplicator.count;
        i++
    ) {
        const ProcessedMessage& message =
            deduplicator.messages[i];

        if (!message.valid) {
            continue;
        }

        Serial.print(
            "ID : "
        );

        Serial.print(
            message.messageId
        );

        Serial.print(
            " | Execution : "
        );

        Serial.print(
            executionStatusToString(
                message.executionStatus
            )
        );

        Serial.print(
            " | Timestamp : "
        );

        Serial.println(
            message.processedAt
        );
    }

    Serial.println(
        "================================"
    );
}