#include <Arduino.h>

#include "message_tracker.h"
#include "message_manager.h"

void initMessageTracker(MessageTracker& tracker) {
    tracker.count = 0;
}

bool trackMessage(
    MessageTracker& tracker,
    const Message& message
) {
    if (tracker.count >= MAX_PENDING_MESSAGES) {
        Serial.println("Tracker plein.");
        return false;
    }

    // Eviter de suivre deux fois le même message.
    if (findPendingMessage(tracker, message.id) != nullptr) {
        Serial.print("Message deja suivi : ");
        Serial.println(message.id);
        return false;
    }

    PendingMessage& pending =
        tracker.messages[tracker.count];

    pending.message = message;

    pending.waitingForAck = true;
    pending.completed = false;
    pending.timedOut = false;

    pending.retryCount = 0;

    pending.sentAt = millis();
    pending.completedAt = 0;

    tracker.count++;

    Serial.print("Message suivi : ");
    Serial.println(message.id);

    return true;
}

PendingMessage* findPendingMessage(
    MessageTracker& tracker,
    uint32_t messageId
) {
    for (uint8_t i = 0; i < tracker.count; i++) {

        if (tracker.messages[i].message.id == messageId) {
            return &tracker.messages[i];
        }
    }

    return nullptr;
}

bool processAck(
    MessageTracker& tracker,
    const Message& ack
) {
    if (ack.type != MessageType::ACK) {
        return false;
    }

    // value2 contient l'ID du message original.
    uint32_t originalMessageId =
        static_cast<uint32_t>(ack.value2);

    PendingMessage* pending =
        findPendingMessage(
            tracker,
            originalMessageId
        );

    if (pending == nullptr) {

        Serial.print(
            "ACK recu pour message inconnu : "
        );

        Serial.println(originalMessageId);

        return false;
    }

    pending->waitingForAck = false;
    pending->completed = true;

    /*
     * Un ACK valide signifie que le message
     * a atteint le destinataire.
     *
     * timedOut reste false.
     */
    pending->timedOut = false;

    pending->completedAt = millis();

    pending->message.status =
        MessageStatus::DELIVERED;

    pending->message.executionStatus =
        ack.executionStatus;

    Serial.print(
        "ACK associe au message : "
    );

    Serial.println(originalMessageId);

    Serial.print(
        "Execution ACK : "
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
    bool changed = false;

    uint32_t now = millis();

    for (uint8_t i = 0; i < tracker.count; i++) {

        PendingMessage& pending =
            tracker.messages[i];

        /*
         * Rien a faire si le message est deja termine
         * ou ne cherche plus d'ACK.
         */
        if (
            !pending.waitingForAck ||
            pending.completed
        ) {
            continue;
        }

        if (
            now - pending.sentAt <
            MESSAGE_TIMEOUT
        ) {
            continue;
        }

        Serial.print(
            "TIMEOUT message : "
        );

        Serial.println(
            pending.message.id
        );

        pending.timedOut = true;

        /*
         * Il reste des tentatives ?
         */
        if (
            pending.retryCount <
            MAX_MESSAGE_RETRIES
        ) {

            pending.retryCount++;

            pending.sentAt = now;

            pending.message.status =
                MessageStatus::PENDING;

            pending.message.executionStatus =
                ExecutionStatus::NOT_EXECUTED;

            /*
             * On remet exactement le même message
             * dans le bus.
             *
             * L'ID reste identique.
             */
            Message retryMessage =
                pending.message;

            if (
                sendMessage(
                    communication,
                    retryMessage
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

                changed = true;
            }
            else {

                Serial.println(
                    "Impossible d'envoyer le retry."
                );
            }
        }

        /*
         * Plus aucune tentative disponible.
         */
        else {

            pending.waitingForAck = false;
            pending.completed = true;

            pending.message.status =
                MessageStatus::FAILED;

            pending.message.executionStatus =
                ExecutionStatus::FAILED;

            pending.completedAt = now;

            Serial.print(
                "ECHEC DEFINITIF message : "
            );

            Serial.println(
                pending.message.id
            );

            changed = true;
        }
    }

    return changed;
}

void printPendingMessage(
    const PendingMessage& pending
) {
    Serial.println(
        "------------------------------"
    );

    Serial.print("ID message      : ");
    Serial.println(
        pending.message.id
    );

    Serial.print("Status          : ");
    Serial.println(
        messageStatusToString(
            pending.message.status
        )
    );

    Serial.print("Execution       : ");
    Serial.println(
        executionStatusToString(
            pending.message.executionStatus
        )
    );

    Serial.print("Waiting ACK     : ");
    Serial.println(
        pending.waitingForAck
            ? "OUI"
            : "NON"
    );

    Serial.print("Completed       : ");
    Serial.println(
        pending.completed
            ? "OUI"
            : "NON"
    );

    Serial.print("Timed out       : ");
    Serial.println(
        pending.timedOut
            ? "OUI"
            : "NON"
    );

    Serial.print("Retry count     : ");
    Serial.println(
        pending.retryCount
    );

    Serial.println(
        "------------------------------"
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

    for (uint8_t i = 0; i < tracker.count; i++) {

        printPendingMessage(
            tracker.messages[i]
        );
    }

    Serial.println(
        "==========================="
    );
}