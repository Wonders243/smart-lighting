#pragma once

#include <Arduino.h>

#include "message.h"


/*
 * ============================================================
 * COMMUNICATION TRANSPORT
 * ============================================================
 *
 * Cette interface représente le moyen physique/logique
 * utilisé pour transporter les messages.
 *
 * L'application ne doit jamais dépendre directement de :
 *
 * - ESP-NOW
 * - Zigbee
 * - Wi-Fi
 * - UART
 *
 * Elle utilise uniquement cette interface.
 */

class CommunicationTransportInterface {

public:

    virtual ~CommunicationTransportInterface() = default;


    /*
     * Initialisation du transport.
     */
    virtual bool begin() = 0;


    /*
     * Envoi.
     */
    virtual bool send(
        const Message& message
    ) = 0;


    /*
     * Réception.
     */
    virtual bool receive(
        Message& message
    ) = 0;


    /*
     * Transport disponible ?
     */
    virtual bool isReady() const = 0;


    /*
     * Nom du transport.
     */
    virtual const char* name() const = 0;
};