#include <Arduino.h>

#include "message_tracker.h"
#include "message_manager.h"

void initMessageTracker(
    MessageTracker& tracker
) {
    tracker.count = 0;
}

bool trackMessage(
    MessageTracker& tracker,
    const Message& message
) {
    if (tracker.count >= MAX_PENDING_MESSAGES) {
        Serial.println("Tracker plein");
        return false;
    }

    if (
        findPendingMessage(
            tracker,
            message.id
        ) != nullptr
    ) {
        Serial.print(
            "Message deja suivi : "
        );

        Serial.println(message.id);

        return false;
    }

    PendingMessage& pending =
        tracker.messages[tracker.count];

    pending.message = message;

    pending.waitingForAck =
        message.type == MessageType::COMMAND;

    pending.completed = false;
    pending.timedOut = false;

    pending.retryCount = 0;

    pending.sentAt = millis();
    pending.completedAt = 0;

    tracker.count++;

    Serial.print(
        "Message suivi : "
    );

    Serial.println(
        message.id
    );

    return true;
}

PendingMessage* findPendingMessage(
    MessageTracker& tracker,
    uint32_t messageId
) {
    for (
        uint8_t i = 0;
        i < tracker.count;
        i++
    ) {
        if (
            tracker.messages[i]
                .message.id
            == messageId
        ) {
            return &tracker.messages[i];
        }
    }

    return nullptr;
}

bool processAck(
    MessageTracker& tracker,
    const Message& ack
) {
    if (
        ack.type != MessageType::ACK
    ) {
        return false;
    }

    uint32_t originalMessageId =
        static_cast<uint32_t>(
            ack.value2
        );

    PendingMessage* pending =
        findPendingMessage(
            tracker,
            originalMessageId
        );

    if (pending == nullptr) {
        Serial.print(
            "Message original introuvable : "
        );

        Serial.println(
            originalMessageId
        );

        return false;
    }

    pending->waitingForAck = false;
    pending->completed = true;
    pending->timedOut = false;

    pending->completedAt = millis();

    pending->message.executionStatus =
        ack.executionStatus;

    if (
        ack.executionStatus
        == ExecutionStatus::EXECUTED
    ) {
        pending->message.status =
            MessageStatus::DELIVERED;
    }
    else {
        pending->message.status =
            MessageStatus::FAILED;
    }

    Serial.print(
        "ACK associe au message : "
    );

    Serial.println(
        originalMessageId
    );

    Serial.print(
        "Execution finale : "
    );

    Serial.println(
        executionStatusToString(
            ack.executionStatus
        )
    );

    return true;
}

bool updateMessageTimeouts(
    MessageTracker& tracker,
    CommunicationBus& communication
) {
    bool retryTriggered = false;

    uint32_t now = millis();

    for (
        uint8_t i = 0;
        i < tracker.count;
        i++
    ) {
        PendingMessage& pending =
            tracker.messages[i];

        if (!pending.waitingForAck) {
            continue;
        }

        if (
            now - pending.sentAt
            < MESSAGE_TIMEOUT
        ) {
            continue;
        }

        /*
         * TIMEOUT
         */

        Serial.print(
            "TIMEOUT message : "
        );

        Serial.println(
            pending.message.id
        );

        /*
         * Peut-on encore réessayer ?
         */

        if (
            pending.retryCount
            < MAX_MESSAGE_RETRIES
        ) {
            pending.retryCount++;

            pending.sentAt = now;
            pending.timedOut = true;

            /*
             * Le message reste en attente
             * d'un ACK.
             */

            pending.waitingForAck = true;

            Message retry =
                pending.message;

            retry.status =
                MessageStatus::PENDING;

            /*
             * IMPORTANT :
             *
             * Le retry utilise le même ID.
             *
             * On ne crée donc pas une
             * nouvelle commande logique.
             */

            if (
                sendMessage(
                    communication,
                    retry
                )
            ) {
                Serial.print(
                    "RETRY #"
                );

                Serial.print(
                    pending.retryCount
                );

                Serial.print(
                    " message : "
                );

                Serial.println(
                    pending.message.id
                );

                retryTriggered = true;
            }
        }
        else {
            /*
             * Nombre maximum de retries atteint.
             */

            pending.waitingForAck = false;
            pending.completed = true;
            pending.timedOut = true;

            pending.message.status =
                MessageStatus::FAILED;

            pending.message.executionStatus =
                ExecutionStatus::FAILED;

            pending.completedAt = now;

            Serial.print(
                "MAX RETRIES atteint : "
            );

            Serial.println(
                pending.message.id
            );

            Serial.print(
                "Message FAILED : "
            );

            Serial.println(
                pending.message.id
            );
        }
    }

    return retryTriggered;
}

void printPendingMessage(
    const PendingMessage& pending
) {
    Serial.println(
        "===== PENDING MESSAGE ====="
    );

    printMessage(
        pending.message
    );

    Serial.print(
        "Waiting ACK : "
    );

    Serial.println(
        pending.waitingForAck
            ? "OUI"
            : "NON"
    );

    Serial.print(
        "Completed : "
    );

    Serial.println(
        pending.completed
            ? "OUI"
            : "NON"
    );

    Serial.print(
        "Timed out : "
    );

    Serial.println(
        pending.timedOut
            ? "OUI"
            : "NON"
    );

    Serial.print(
        "Retry count : "
    );

    Serial.println(
        pending.retryCount
    );

    Serial.print(
        "Sent at : "
    );

    Serial.println(
        pending.sentAt
    );

    Serial.print(
        "Completed at : "
    );

    Serial.println(
        pending.completedAt
    );

    Serial.println(
        "==========================="
    );
}

void printMessageTracker(
    const MessageTracker& tracker
) {
    Serial.println();
    Serial.println(
        "===== MESSAGE TRACKER ====="
    );

    Serial.print(
        "Messages suivis : "
    );

    Serial.println(
        tracker.count
    );

    for (
        uint8_t i = 0;
        i < tracker.count;
        i++
    ) {
        printPendingMessage(
            tracker.messages[i]
        );
    }

    Serial.println(
        "==========================="
    );
}