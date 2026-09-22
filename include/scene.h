#pragma once

#include <Arduino.h>

#include "command.h"


// ============================================================
// CONFIGURATION
// ============================================================

constexpr uint8_t MAX_SCENE_ACTIONS = 10;


// ============================================================
// STATUT GLOBAL D'UNE SCENE
// ============================================================

enum class SceneExecutionStatus {

    EXECUTED,
    PARTIAL,
    FAILED
};


// ============================================================
// ACTION D'UNE SCENE
// ============================================================

struct SceneAction {

    uint32_t groupId;

    CommandType commandType;

    int32_t value;
};


// ============================================================
// SCENE
// ============================================================

struct Scene {

    uint32_t id;

    const char* name;

    SceneAction actions[MAX_SCENE_ACTIONS];

    uint8_t actionCount;
};