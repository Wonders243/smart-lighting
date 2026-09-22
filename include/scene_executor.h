#pragma once

#include "scene.h"
#include "scene_manager.h"
#include "group_manager.h"
#include "device_registry.h"


// ============================================================
// RESULTAT D'EXECUTION
// ============================================================

struct SceneExecutionResult {

    SceneExecutionStatus status;

    uint8_t actionsExecuted;

    uint8_t actionsPartial;

    uint8_t actionsFailed;

    uint8_t lampsAffected;

    uint8_t lampsSkipped;
};


// ============================================================
// EXECUTION
// ============================================================

SceneExecutionStatus executeScene(

    SceneRegistry& scenes,

    GroupRegistry& groups,

    LampRegistry& lamps,

    uint32_t sceneId,

    SceneExecutionResult& result
);