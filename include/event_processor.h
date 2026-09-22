#pragma once

#include "event_bus.h"

#include "automation_engine.h"
#include "automation_manager.h"

#include "scene_manager.h"
#include "group_manager.h"
#include "device_registry.h"

#include "automation_context.h"

void processEvents(
    EventBus& bus,
    AutomationRegistry& automations,
    SceneRegistry& scenes,
    GroupRegistry& groups,
    LampRegistry& lamps,
    const AutomationContext& context
);