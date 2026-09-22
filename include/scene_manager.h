#pragma once

#include <Arduino.h>

#include "scene.h"
#include "group_manager.h"
#include "device_registry.h"


// ============================================================
// CONFIGURATION
// ============================================================

constexpr uint8_t MAX_SCENES = 10;


// ============================================================
// REGISTRE DES SCENES
// ============================================================

struct SceneRegistry {

    Scene scenes[MAX_SCENES];

    uint8_t count;
};


// ============================================================
// INITIALISATION
// ============================================================

void initSceneRegistry(
    SceneRegistry& registry
);


// ============================================================
// GESTION DES SCENES
// ============================================================

bool addScene(
    SceneRegistry& registry,
    const Scene& scene
);


bool removeScene(
    SceneRegistry& registry,
    uint32_t sceneId
);


Scene* findScene(
    SceneRegistry& registry,
    uint32_t sceneId
);


// ============================================================
// GESTION DES ACTIONS
// ============================================================

bool addActionToScene(
    SceneRegistry& scenes,
    GroupRegistry& groups,
    uint32_t sceneId,
    const SceneAction& action
);


bool removeActionFromScene(
    SceneRegistry& scenes,
    uint32_t sceneId,
    uint8_t actionIndex
);


// ============================================================
// AFFICHAGE
// ============================================================

void printScene(
    const Scene& scene
);


void printSceneRegistry(
    const SceneRegistry& registry
);