#pragma once

#include "action.h"
#include "scene_manager.h"
#include "group_manager.h"
#include "device_registry.h"

bool executeAction(
    const Action& action,
    SceneRegistry& scenes,
    GroupRegistry& groups,
    LampRegistry& lamps
);