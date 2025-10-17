//
// Created by DirgeWuff on 6/27/25.
//

#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include "box2d/box2d.h"
#include "raylib.h"
#include "Tson/tileson.hpp"

bool isShapeIdEqual(const b2ShapeId& idOne, const b2ShapeId& idTwo);

Vector2 metersToPixelsVec(const b2Vec2& vec);
b2Vec2 pixelsToMetersVec(const Vector2& vec);
float metersToPixels(float meters);
float pixelsToMeters(float pixels);

Vector2 toRayVec2(const tson::Vector2f& vec);
b2Vec2 toB2Vec2(const tson::Vector2i& vec);
Rectangle toRayRect(const tson::Rect& rect);
float getRandFloat(float min, float max);
int getRandInt(int min, int max);

tson::Vector2i v2iAdd(
    const tson::Vector2i& vecOne,
    const tson::Vector2i& vecTwo);

// Code timer class. Can be used to track and display the time taken by a function call or other operation.
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

#endif //UTILS_H