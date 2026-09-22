#pragma once

#include "communication.h"
#include "device_registry.h"
#include "group_manager.h"
#include "scene_manager.h"

void processMessages(
    CommunicationBus& communication,
    LampRegistry& lamps,
    GroupRegistry& groups,
    SceneRegistry& scenes
);