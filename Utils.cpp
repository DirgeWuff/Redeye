//
// Created by DirgeWuff on 6/27/25.
//

#include "Utils.h"

constexpr uint8_t PPM = 100; // 100 px/meter

bool isShapeIdEqual(const b2ShapeId& idOne, const b2ShapeId& idTwo) {
    return idOne.generation == idTwo.generation &&
           idOne.index1 == idTwo.index1 &&
           idOne.world0 == idTwo.world0;
}

Vector2 metersToPixelsVec(const b2Vec2& vec) {
    return Vector2{vec.x * PPM, vec.y * PPM};
}

b2Vec2 pixelsToMetersVec(const Vector2& vec) {
    return b2Vec2{vec.x / PPM, vec.y / PPM};
}

float metersToPixels(const float meters) {
    return meters * PPM;
}

float pixelsToMeters(const float pixels) {
    return pixels / PPM;
}

b2Vec2 toB2Vec2(const tson::Vector2i &vec) {
    return b2Vec2{
    pixelsToMeters(vec.x),
    pixelsToMeters(vec.y)
    };
}

Vector2 toRayVec2(const tson::Vector2f &vec) {
    return Vector2{
        vec.x,
        vec.y
    };
}

Rectangle toRayRect(const tson::Rect &rect) {
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

CodeClock::~CodeClock() {
    std::chrono::duration<float> totalTime = {};

    for (const auto& time : durations) {
        totalTime += time;
    }

    const std::chrono::duration<float> averageTime = totalTime / durations.size();
    std::cout << "Average time: " << averageTime.count() << "s." << std::endl;
}


void CodeClock::begin() {
    startingTime = std::chrono::high_resolution_clock::now();
}

void CodeClock::end() {
    endingTime = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> duration = startingTime - endingTime;
    durations.push_back(duration);
}


