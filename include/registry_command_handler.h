#pragma once

#include "device_registry.h"
#include "command.h"


bool executeRegistryCommand(
    LampRegistry& registry,
    Command& command
);