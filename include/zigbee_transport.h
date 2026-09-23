#pragma once

#include "communication_transport.h"


/*
 * ============================================================
 * ZIGBEE TRANSPORT
 * ============================================================
 *
 * V3.9 :
 * ----------
 * Architecture préparée.
 *
 * L'implémentation réelle sera réalisée sur ESP32-C6/H2
 * avec la stack Zigbee d'Espressif.
 *
 * La bibliothèque Arduino ESP32 fournit déjà :
 *
 * - Coordinator
 * - Router
 * - End Device
 * - commissioning
 * - network management
 * - endpoints
 * - OTA
 * - groups
 * - binding
 *
 * Voir documentation Espressif.
 */

class ZigbeeTransport
    : public CommunicationTransportInterface {

private:

    bool ready;


public:

    ZigbeeTransport();


    bool begin() override;


    bool send(
        const Message& message
    ) override;


    bool receive(
        Message& message
    ) override;


    bool isReady() const override;


    const char* name() const override;
};