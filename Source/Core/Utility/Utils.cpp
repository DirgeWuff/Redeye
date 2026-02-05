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
// Definitions of non-templated utility functions from Utils.H

#include <cstdint>
#include <random>
#include "box2d/types.h"
#include "Utils.h"
#include "Globals.h"

namespace RE::Core {
    // Equality comparison
    //======================================================================================================================
    bool isShapeIdEqual(const b2ShapeId& idOne, const b2ShapeId& idTwo) {
        return idOne.generation == idTwo.generation &&
               idOne.index1 == idTwo.index1 &&
               idOne.world0 == idTwo.world0;
    }

    // Number conversion
    // =====================================================================================================================
    Vector2 metersToPixelsVec(const b2Vec2& vec) {
        return Vector2{vec.x * g_ppm, vec.y * g_ppm};
    }

    float metersToPixels(const float meters) {
        return meters * g_ppm;
    }

    float pixelsToMeters(const float pixels) {
        return pixels / g_ppm;
    }

    // Misc math
    // =====================================================================================================================
    tson::Vector2i v2iAdd(
        const tson::Vector2i& vecOne,
        const tson::Vector2i& vecTwo) {
        return tson::Vector2i {
            vecOne.x + vecTwo.x,
            vecOne.y + vecTwo.y
        };
    }

    // RNG
    // =====================================================================================================================
    float getRandFloat(float min, float max) {
        if (min > max)
            std::swap(min, max);

        std::uniform_real_distribution distribution(min, max);
        return distribution(g_randomGenerator);
    }

    int getRandInt(int min, int max) {
        if (min > max)
            std::swap(min, max);

        std::uniform_int_distribution distribution(min, max);
        return distribution(g_randomGenerator);
    }

    // Timing and performance
    // =====================================================================================================================
    // TODO: Add more robust and varied timing/perf functions
    CodeClock::~CodeClock() {
        std::chrono::duration<float> totalTime = {};

        for (const auto& time : m_durations) {
            totalTime += time;
        }

        const std::chrono::duration<float> averageTime = totalTime / m_durations.size();
        const double seconds = averageTime.count();

        // Format to be more readable in case the time is REALLY fast.
        // Using scientific notation here
        if (seconds < 1e-3) {
            std::cout << "Average time: " << seconds * 1e6 << " microseconds." << std::endl;
        }
        else if (seconds < 1) {
            std::cout << "Average time: " << seconds * 1e3 << "milliseconds" << std::endl;
        }
        else {
            std::cout << "Average time: " << seconds << "s." << std::endl;
        }
    }

    void CodeClock::begin() {
        m_startingTime = std::chrono::high_resolution_clock::now();
    }

    void CodeClock::end() {
        m_endingTime = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float> duration = m_endingTime - m_startingTime;
        m_durations.push_back(duration);
    }

    // identification
    // =====================================================================================================================
    guid generateGuid() {
        std::uniform_int_distribution<std::uint64_t> distribution;

        return guid{distribution(g_randomGenerator)};
    }
}