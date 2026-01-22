//
// Created by DirgeWuff on 11/10/2025.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdint>
#include <string>
#include <random>
#include "raylib.h"

static constexpr uint16_t g_windowWidth = 1500;
static constexpr uint16_t g_windowHeight = 800;
// Replace this with a serialized config later...
inline std::string g_playerSpritePath = "../assets/Player assets/Walksprites_v5.png";

constexpr double g_pi = 3.14159265359;

constexpr float g_worldStep = 1.0f / 60.0f;
constexpr std::uint8_t g_subStep = 4;

constexpr float g_playerWalkMultiplier = 0.50f;
constexpr float g_playerJumpMultiplier = 16.0f;
constexpr float g_slopeForceMultiplier = 2.50f;

constexpr std::uint64_t g_universalMaskBits = 0xFFFF;
constexpr std::uint64_t g_playerCategoryBits = 0x0001;
constexpr std::uint64_t g_footpawCategoryBits = 0x0002;
constexpr std::uint64_t g_groundCategoryBits = 0x0004;
constexpr std::uint64_t g_raycastCategoryBits = 0x0008;

const Font g_debugFont = LoadFont("../assets/Fonts/JetBrainsMono-Bold.ttf");

inline bool g_debugWindowBoxActive = false;
inline bool g_drawPlayerShapes = false;
inline bool g_drawPlayerSensorStatus = false;
inline bool g_drawPlayerPos = false;
inline bool g_drawPlayerCenter = false;
inline bool g_drawTerrainShapes = false;
inline bool g_drawTerrainVerts = false;
inline bool g_drawCameraCrosshair = false;
inline bool g_drawCameraRect = false;
inline bool g_drawEventColliders = false;
inline bool g_drawShaderEffects = true;
inline bool g_drawPlayerAnimId = false;
inline bool g_drawPlayerActionState = false;

constexpr Color g_debugBodyColor{0, 0, 255, 255};
constexpr Color g_debugCollisionColor{255, 0, 0, 255};
constexpr Color g_debugColliderColor{5, 237, 16, 255};
constexpr Color g_debugVertColor{181, 2, 157, 255};

constexpr int g_debugTextSize = 20;
constexpr int g_totalDebugTextHeight = 40; // total text row height including spacing
constexpr Vector2 g_debugTextPos = {10.0f, 10.0f};

constexpr uint8_t g_ppm = 100; // 100 px/meter

inline std::random_device g_randomDevice;
inline std::mt19937 g_randomGenerator(g_randomDevice());

inline std::string g_saveFolderPath = "../Savegame";
inline std::string g_saveFilePath = g_saveFolderPath + "/save.toml";

// Might wanna tweak these to make animations smoother at some point...
constexpr float g_buttonPosXScaleFactor = 0.02f;
constexpr float g_buttonPosYScaleFactor = 0.004f;
constexpr float g_buttonWidthScaleFactor = 0.97f;
constexpr float g_buttonHeightScaleFactor = 0.98f;
constexpr float g_buttonTextScaleFactor = 0.30f;

constexpr float g_beamOffsetXRight = 123.5f;
constexpr float g_beamOffsetXLeft = 40.0f;
constexpr float g_beamOffsetY = 110.0f;

#endif //GLOBALS_H
