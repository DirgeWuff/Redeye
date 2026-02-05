//
// Author: DirgeWuff
// Created on: 6/27/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Collection of declarations of various generic utility functions, as well
// as definitions of templated utility functions.


#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <type_traits>
#include <hash_map>
#include "Logging.h"
#include "box2d/box2d.h"
#include "raylib.h"
#include "Tson/tileson.hpp"
#include "Toml/toml.hpp"

// This stuff for GUID must be declared in global namespace now because hash<guid> is a template specialization
struct guid {
    std::uint64_t val{};

    bool operator==(const guid& other) const { return this->val == other.val; }
    bool operator!=(const guid& other) const { return this->val != other.val; }
};

namespace std { // NOLINT
    template<>
struct hash<guid> {
        std::size_t operator()(const guid& id) const noexcept {
            return std::hash<uint64_t>{}(id.val);
        }
    };
}

namespace RE::Core {
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

    template<typename T, typename K>
    T getValFromToml(const toml::table& tbl, const K& key) {
        const std::optional<T> val = tbl[std::string(key)].value<T>();

        if (!val.has_value()) {
            logDbg("Failed to get value from TOML key: ", key);
            return{};
        }

        return val.value();
    }

    // This all is a bit of an abomination, but it works...
    template<typename T, typename H, typename K>
    T getNestedValFromToml(const toml::table& tbl, const H& header, const K& key) {
        const std::optional<T> val = tbl[std::string(header)][std::string(key)].value<T>();

        if (!val.has_value()) {
            logDbg("Failed to retrieve TOML key: ", key, " from header ", header);
            return{};
        }

        return val.value();
    }

    // Not really possible to safely return type T here, since Vector2 is already defined as {float x, float y}
    template<typename K>
    Vector2 getVecFromToml(const toml::table& tbl, const K& key) {
        const auto* arr = tbl[std::string(key)].as_array();

        if (!arr) {
            logDbg("arr == nullptr. getVecFromToml(Args...)");
            return{};
        }

        if (arr->size() != 2) {
            logDbg("Array at key ", key, " is not of size 2. getVecFromToml(Args...)");
            return{};
        }

        const std::optional<float> x = arr->at(0).value<float>();
        const std::optional<float> y = arr->at(1).value<float>();

        if (!x.has_value() || !y.has_value()) {
            logDbg("Unable to retrieve one or more values from key ", key, "getVecFromToml(Args...)");
            return{};
        }

        return {x.value(), y.value()};
    }

    template<typename H, typename K>
    Vector2 getNestedVecFromToml(const toml::table& tbl, const H& header, const K& key) {
        const auto* arr = tbl[std::string(header)][std::string(key)].as_array();

        if (!arr) {
            logDbg("arr == nullptr. getNestedVecFromToml(Args...)");
            return{};
        }

        if (arr->size() != 2) {
            logDbg("Array at header ", std::string(header), " and key ", std::string(key),
                " is not of size 2. getNestedVecFromToml(Args...)");
            return{};
        }

        const std::optional<float> x = arr->at(0).value<float>();
        const std::optional<float> y = arr->at(1).value<float>();

        if (!x.has_value() || !y.has_value()) {
            logDbg("Unable to retrieve one or more values from header ",
                std::string(header), " and key ", std::string(key));
            return{};
        }

        return{x.value(), y.value()};
    }

    template<typename T, typename K>
    std::vector<T> getArrFromToml(const toml::table& tbl, const K& key) {
        const auto* arr = tbl[std::string(key)].as_array();

        if (!arr) {
            logDbg("arr == nullptr. getArrFromToml(Args...)");
            return{};
        }

        if (arr->empty()) {
            logDbg("Array at key ", key, " is empty.");
            return{};
        }

        std::vector<T> vec{};
        vec.reserve(arr->size());

        for (const auto& elem : *arr) {
            if (auto val = elem.value<T>())
                vec.push_back(*val);
            else
                logDbg("Invalid type stored in array at key: ", key, ". getArrFromToml(Args...)");
        }

        return vec;
    }

    // Identification
    // =====================================================================================================================
    guid generateGuid();

}

#endif //UTILS_H