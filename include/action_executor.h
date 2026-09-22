#pragma once

#include "action.h"
#include "message.h"
#include "scene_manager.h"
#include "group_manager.h"
#include "device_registry.h"

ExecutionStatus executeAction(
    const Action& action,
    SceneRegistry& scenes,
    GroupRegistry& groups,
    LampRegistry& lamps
);