//
// Created by DirgeWuff on 6/27/25.
//

#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <type_traits>
#include "Error.h"
#include "box2d/box2d.h"
#include "raylib.h"
#include "Tson/tileson.hpp"
#include "Toml/toml.hpp"

// Equality comparison
//======================================================================================================================
bool isShapeIdEqual(const b2ShapeId& idOne, const b2ShapeId& idTwo);

// Number conversion
// =====================================================================================================================
Vector2 metersToPixelsVec(const b2Vec2& vec);
float metersToPixels(float meters);
float pixelsToMeters(float pixels);

// Type conversion
// =====================================================================================================================

// After the recent changes, the naming of these functions is confusing me...
// But I sat here for ten minutes staring at it and couldn't think of anything better.
template<typename T>
Vector2 toRayVec2(const T& vec) {
    using noConstRef = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(!std::is_same_v<noConstRef, Vector2>);
    static_assert(!std::is_same_v<noConstRef, b2Vec2>);

    return {
        static_cast<float>(vec.x),
        static_cast<float>(vec.y)};
}

template<typename T>
b2Vec2 pixelsToMetersVec(const T& vec) {
    using noConstRef = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(!std::is_same_v<noConstRef, b2Vec2>);

    return {
        pixelsToMeters(static_cast<float>(vec.x)),
        pixelsToMeters(static_cast<float>(vec.y))};
}

template<typename T>
Rectangle toRayRect(const T& rect) {
    using noConstRef = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(!std::is_same_v<noConstRef, Rectangle>);

    return Rectangle{
        static_cast<float>(rect.x),
        static_cast<float>(rect.y),
        static_cast<float>(rect.width),
        static_cast<float>(rect.height)};
}

// RNG
// =====================================================================================================================
float getRandFloat(float min, float max);
int getRandInt(int min, int max);

tson::Vector2i v2iAdd(
    const tson::Vector2i& vecOne,
    const tson::Vector2i& vecTwo);

// Timing and performance
// =====================================================================================================================
class CodeClock {
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startingTime{};
    std::chrono::time_point<std::chrono::high_resolution_clock> m_endingTime{};
    std::vector<std::chrono::duration<float>> m_durations{};

public:
    CodeClock() = default;
    ~CodeClock();
    void begin();
    void end();
};

// Toml helpers
// =====================================================================================================================

// This all is a bit of an abomination, but it works...
template<typename H, typename K>
std::string getNestedStringFromToml(const toml::table& tbl,H& headerKey, K& key ) {
    const std::optional<std::string> str = tbl[std::string(headerKey)][std::string(key)].value<std::string>();

    if (!str.has_value()) {
        logErr(std::string("Table contains no value \"" + std::string(key) +
            std::string("\" matching type std::string under specified header. Utils::getNestedStringFromToml(Args...)")));
        return {};
    }

    return str.value();
}

template<typename H, typename K>
float getNestedFloatFromToml(const toml::table& tbl, H& headerKey, K& key) {
    const std::optional<float> fl = tbl[std::string(headerKey)][std::string(key)].value<float>();

    if (!fl.has_value()) {
        logErr(std::string("Table contains no value \"" + std::string(key) +
            std::string("\", matching type float under specified header. Utils::getNestedFloatFromToml(Args...)")));
        return {};
    }

    return fl.value();
}

#endif //UTILS_H