//
// Created by DirgeWuff on 6/27/25.
//

#ifndef B2UTILS_H
#define B2UTILS_H

#include <chrono>
#include "box2d/box2d.h"
#include "raylib.h"
#include "../external_libs/Tson/tileson.hpp"

bool isShapeIdEqual(const b2ShapeId& idOne, const b2ShapeId& idTwo);    // Check if 2 b2ShapeId's are equal

Vector2 metersToPixelsVec(const b2Vec2& vec);                           // Convert b2 meters to rlpixels in vector2
b2Vec2 pixelsToMetersVec(const Vector2& vec);                           // Convert rl pixels to b2 meters in vector2
float metersToPixels(float meters);                                     // Convert b2 meters to rl pixels
float pixelsToMeters(float pixels);                                     // Convert rl pixels to b2 meters

Vector2 toRayVec2(const tson::Vector2f& vec);                           // Convert tson::Vector2i to Vector2
b2Vec2 toB2Vec2(const tson::Vector2i& vec);                             // Convert tson::Vector2i to b2Vec2
Rectangle toRayRect(const tson::Rect& rect);                            // Convert tson::Rect to Rectangle

tson::Vector2i v2iAdd(
    const tson::Vector2i& vecOne,
    const tson::Vector2i& vecTwo);                                      // Perform addition with tson::Vector2i

// Code timer class. Can be used to track and display the time taken by a function call or other operation.
class CodeClock {
    std::chrono::time_point<std::chrono::high_resolution_clock> startingTime{};
    std::chrono::time_point<std::chrono::high_resolution_clock> endingTime{};
    std::vector<std::chrono::duration<float>> durations{};

public:
    CodeClock() = default;
    ~CodeClock();
    void begin(); // Start the CodeClock.
    void end();   // Stop the CodeClock and print the average time for the operation.
};

#endif //B2UTILS_H