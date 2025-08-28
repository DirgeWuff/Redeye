//
// Created by DirgeWuff on 6/27/25.
//

#include <cstdint>
#include <random>
#include "box2d/types.h"
#include "Utils.h"

// Globals
//======================================================================================================================
constexpr uint8_t g_ppm = 100; // 100 px/meter

std::random_device g_randomDevice;
std::mt19937 g_randomGenerator(g_randomDevice());

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

b2Vec2 pixelsToMetersVec(const Vector2& vec) {
    return b2Vec2{vec.x / g_ppm, vec.y / g_ppm};
}

float metersToPixels(const float meters) {
    return meters * g_ppm;
}

float pixelsToMeters(const float pixels) {
    return pixels / g_ppm;
}

// Type conversion
// =====================================================================================================================
b2Vec2 toB2Vec2(const tson::Vector2i& vec) {
    return b2Vec2{
    pixelsToMeters(vec.x),
    pixelsToMeters(vec.y)
    };
}

Vector2 toRayVec2(const tson::Vector2f& vec) {
    return Vector2{
        vec.x,
        vec.y
    };
}

Rectangle toRayRect(const tson::Rect& rect) {
    return Rectangle{
    static_cast<float>(rect.x),
    static_cast<float>(rect.y),
    static_cast<float>(rect.width),
    static_cast<float>(rect.height)
    };
}

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

    std::uniform_real_distribution<>distribution(min, max);
    return distribution(g_randomGenerator);
}

int getRandInt(int min, int max) {
    if (min > max)
        std::swap(min, max);

    std::uniform_int_distribution<> distribution(min, max);
    return distribution(g_randomGenerator);
}

// Timing and performance
// =====================================================================================================================
// TODO: Add more robust and varied timing/perf functions
CodeClock::~CodeClock() {
    std::chrono::duration<float> totalTime = {};

    for (const auto& time : durations) {
        totalTime += time;
    }

    const std::chrono::duration<float> averageTime = totalTime / durations.size();
    double seconds = averageTime.count();

    // Format to be more readable in case the time is REALLY fast.
    // Using scientific notation here
    if (seconds < 1e-3) {
        std::cout << "Average time: " << seconds * 1e6 << " micro seconds." << std::endl;
    }
    else if (seconds < 1) {
        std::cout << "Average time: " << seconds * 1e3 << "milliseconds" << std::endl;
    }
    else {
        std::cout << "Average time: " << seconds << "s." << std::endl;
    }
}

void CodeClock::begin() {
    startingTime = std::chrono::high_resolution_clock::now();
}

void CodeClock::end() {
    endingTime = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> duration = endingTime - startingTime;
    durations.push_back(duration);
}
