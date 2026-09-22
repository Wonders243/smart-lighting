#pragma once

#include <Arduino.h>

#include "group.h"
#include "device_registry.h"


// ============================================================
// CONFIGURATION
// ============================================================

constexpr uint8_t MAX_GROUPS = 10;


// ============================================================
// REGISTRE DES GROUPES
// ============================================================

struct GroupRegistry {

    LampGroup groups[MAX_GROUPS];

    uint8_t count;
};


// ============================================================
// INITIALISATION
// ============================================================

void initGroupRegistry(
    GroupRegistry& registry
);


// ============================================================
// GROUPES
// ============================================================

bool addGroup(
    GroupRegistry& registry,
    const LampGroup& group
);


bool removeGroup(
    GroupRegistry& registry,
    uint32_t groupId
);


LampGroup* findGroup(
    GroupRegistry& registry,
    uint32_t groupId
);


// ============================================================
// GESTION DES LAMPES DANS UN GROUPE
// ============================================================

bool addLampToGroup(
    GroupRegistry& groups,
    LampRegistry& lamps,
    uint32_t groupId,
    uint32_t lampId
);


bool removeLampFromGroup(
    GroupRegistry& groups,
    uint32_t groupId,
    uint32_t lampId
);


bool lampBelongsToGroup(
    const LampGroup& group,
    uint32_t lampId
);


// ============================================================
// AFFICHAGE
// ============================================================

void printGroup(
    const LampGroup& group
);


void printGroupRegistry(
    const GroupRegistry& registry
);