#pragma once

#include "group_manager.h"
#include "command.h"
#include "device_registry.h"

bool executeGroupCommand(
    GroupRegistry& groups,
    LampRegistry& lamps,
    Command& command
);