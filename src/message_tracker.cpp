#include <Arduino.h>

#include "message_tracker.h"
#include "message_manager.h"


void initMessageTracker(
    MessageTracker& tracker
) {
    tracker.count = 0;

    for (
        uint8_t i = 0;
        i < MAX_PENDING_MESSAGES;
        i++
    ) {

        tracker.messages[i].waitingForAck =
            false;

        tracker.messages[i].completed =
            false;

        tracker.messages[i].timedOut =
            false;

        tracker.messages[i].retryCount =
            0;

        tracker.messages[i].sentAt =
            0;

        tracker.messages[i].completedAt =
            0;
    }
}


/*
 * ============================================================
 * TRACK
 * ============================================================
 */

bool trackMessage(
    MessageTracker& tracker,
    const Message& message
) {
    /*
     * Vérification doublon.
     */

    if (
        findPendingMessage(
            tracker,
            message.id
        ) != nullptr
    ) {

        Serial.print(
            "Message deja suivi : "
        );

        Serial.println(
            message.id
        );

        return false;
    }


    if (
        tracker.count >=
        MAX_PENDING_MESSAGES
    ) {

        Serial.println(
            "Tracker plein"
        );

        return false;
    }


    PendingMessage& pending =
        tracker.messages[
            tracker.count
        ];


    pending.message =
        message;

    pending.waitingForAck =
        true;

    pending.completed =
        false;

    pending.timedOut =
        false;

    pending.retryCount =
        0;

    pending.sentAt =
        millis();

    pending.completedAt =
        0;


    tracker.count++;


    Serial.print(
        "Message suivi : "
    );

    Serial.println(
        message.id
    );


    return true;
}


/*
 * ============================================================
 * FIND
 * ============================================================
 */

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
                .message.id ==
            messageId
        ) {

            return
                &tracker.messages[i];
        }
    }


    return nullptr;
}


/*
 * ============================================================
 * ACK
 * ============================================================
 */

bool processAck(
    MessageTracker& tracker,
    const Message& ack
) {
    if (
        ack.type !=
        MessageType::ACK
    ) {
        return false;
    }


    /*
     * value2 contient l'ID
     * du message original.
     */

    uint32_t originalMessageId =
        static_cast<uint32_t>(
            ack.value2
        );


    PendingMessage* pending =
        findPendingMessage(
            tracker,
            originalMessageId
        );


    if (
        pending == nullptr
    ) {

        Serial.print(
            "ACK sans message correspondant : "
        );

        Serial.println(
            originalMessageId
        );

        return false;
    }


    pending->waitingForAck =
        false;

    pending->completed =
        true;

    pending->timedOut =
        false;

    pending->completedAt =
        millis();


    pending->message.status =
        MessageStatus::DELIVERED;


    pending->message.executionStatus =
        ack.executionStatus;


    Serial.print(
        "ACK traite pour message : "
    );

    Serial.println(
        originalMessageId
    );


    return true;
}


/*
 * ============================================================
 * TIMEOUT + RETRY
 * ============================================================
 */

bool updateMessageTimeouts(
    MessageTracker& tracker,
    Communication& communication
) {
    bool changed = false;


    for (
        uint8_t i = 0;
        i < tracker.count;
        i++
    ) {

        PendingMessage& pending =
            tracker.messages[i];


        if (
            !pending.waitingForAck
        ) {
            continue;
        }


        uint32_t elapsed =
            millis() -
            pending.sentAt;


        if (
            elapsed <
            MESSAGE_TIMEOUT
        ) {
            continue;
        }


        changed = true;


        Serial.println();

        Serial.print(
            "TIMEOUT message : "
        );

        Serial.println(
            pending.message.id
        );


        /*
         * RETRY
         */

        if (
            pending.retryCount <
            MAX_MESSAGE_RETRIES
        ) {

            pending.retryCount++;


            pending.sentAt =
                millis();


            pending.message.status =
                MessageStatus::PENDING;


            pending.message.executionStatus =
                ExecutionStatus::NOT_EXECUTED;


            Serial.print(
                "RETRY #"
            );

            Serial.println(
                pending.retryCount
            );


            /*
             * On réutilise exactement
             * le même Message ID.
             *
             * Cela permet au destinataire
             * d'utiliser le deduplicator.
             */

            sendMessage(
                communication,
                pending.message
            );


            continue;
        }


        /*
         * ECHEC DEFINITIF
         */

        pending.waitingForAck =
            false;

        pending.completed =
            true;

        pending.timedOut =
            true;

        pending.completedAt =
            millis();


        pending.message.status =
            MessageStatus::FAILED;


        pending.message.executionStatus =
            ExecutionStatus::FAILED;


        Serial.println(
            "ECHEC DEFINITIF"
        );
    }


    return changed;
}


/*
 * ============================================================
 * PRINT PENDING
 * ============================================================
 */

void printPendingMessage(
    const PendingMessage& pending
) {
    Serial.println();

    Serial.println(
        "===== PENDING MESSAGE ====="
    );


    Serial.print(
        "ID : "
    );

    Serial.println(
        pending.message.id
    );


    Serial.print(
        "Destination : "
    );

    Serial.println(
        pending.message.destinationId
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
        "Retries : "
    );

    Serial.println(
        pending.retryCount
    );


    Serial.print(
        "Status : "
    );

    Serial.println(
        messageStatusToString(
            pending.message.status
        )
    );


    Serial.print(
        "Execution : "
    );

    Serial.println(
        executionStatusToString(
            pending.message.executionStatus
        )
    );


    Serial.println(
        "==========================="
    );
}


/*
 * ============================================================
 * PRINT TRACKER
 * ============================================================
 */

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
        "============================"
    );
}