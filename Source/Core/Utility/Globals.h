//
// Created by DirgeWuff on 11/10/2025.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdint>

constexpr double g_pi = 3.14159265359;

constexpr std::uint64_t g_universalMaskBits = 0xFFFF;
constexpr std::uint64_t g_playerCategoryBits = 0x0001;
constexpr std::uint64_t g_footpawCategoryBits = 0x0002;
constexpr std::uint64_t g_groundCategoryBits = 0x0004;
constexpr std::uint64_t g_raycastCategoryBits = 0x0008;

#endif //GLOBALS_H
