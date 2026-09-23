#pragma once

#include "communication_transport.h"

constexpr uint8_t MAX_SIMULATION_MESSAGES = 20;


class SimulationTransport
    : public CommunicationTransportInterface {

private:

    Message messages[
        MAX_SIMULATION_MESSAGES
    ];

    uint8_t head;

    uint8_t tail;

    uint8_t count;

    bool ready;


public:

    SimulationTransport();


    bool begin() override;


    bool send(
        const Message& message
    ) override;


    bool receive(
        Message& message
    ) override;


    bool isReady() const override;


    const char* name() const override;


    bool empty() const;

    bool full() const;

    uint8_t size() const;
};