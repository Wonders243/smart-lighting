#pragma once

#include "communication.h"
#include "device_registry.h"
#include "group_manager.h"
#include "scene_manager.h"
#include "message_deduplicator.h"
#include "message_tracker.h"

void processMessages(
    CommunicationBus& communication,
    MessageTracker& tracker,
    MessageDeduplicator& deduplicator,
    LampRegistry& lamps,
    GroupRegistry& groups,
    SceneRegistry& scenes,
    bool dropNextAck = false
);