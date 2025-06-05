//
// Created by DirgeWuff on 5/26/25.
//

#include "Camera.h"

SceneCamera::SceneCamera(const TiledMap& map) {
    camera = std::make_shared<Camera2D>();
    this->camera->offset = {1500.0f / 2.0f, 800.0f / 2}; // Hard coded value, no bueno, fix later
    this->camera->zoom = 1.50f;
    this->camera->rotation = 0.0f;

    cameraRect = {
        (GetScreenWidth() - static_cast<float>(GetScreenWidth()) / this->camera->zoom) / 2,
        (GetScreenHeight() - static_cast<float>(GetScreenHeight()) / this->camera->zoom) / 2,
        GetScreenWidth() / this->camera->zoom,
        GetScreenHeight() / this->camera->zoom};

    cameraCenter = {cameraRect.x + cameraRect.width / 2, cameraRect.y + cameraRect.height / 2};
    cameraVelocityY = 15;
    targetCenter = {0.0f, 0.0f};
    mapSize = {
        map.getTileWidth() * map.getMapWidth(),
        map.getTileHeight() * map.getMapHeight()};
}

SceneCamera::~SceneCamera() = default;

// Maybe a bad way of doing this, it locks to center of target no matter the position of target
void SceneCamera::setTarget(const std::shared_ptr<Entity>& targetEntity){
    this->camera->target = {
        targetEntity->getX() + (targetEntity->getWidth() / 2),
        targetEntity->getY() + (targetEntity->getHeight()) / 2};

    targetCenter = {this->camera->target.x, this->camera->target.y};
}

void SceneCamera::update(const std::shared_ptr<Entity>& targetEntity) {
    targetCenter = {
        targetEntity->getX() + targetEntity->getWidth() / 2,
        targetEntity->getY() + targetEntity->getHeight() / 2
    };

    // Clamp camera within map bounds on X axis
    if (targetCenter.x <= cameraRect.width / 2) {
        this->camera->target = this->camera->target;
    }
    else if (
        targetCenter.x >= mapSize.x - cameraRect.width / 2 - targetEntity->getWidth()) {

        this->camera->target = this->camera->target;
    }
    else {
        this->camera->target.x = targetCenter.x;
    }

    // Clamp camera within map bounds on Y axis
    if (targetCenter.y > cameraCenter.y) {
        if (cameraRect.y + cameraRect.height < mapSize.y) {
            this->camera->target.y += cameraVelocityY;
        }
    }
    else if (targetCenter.y < cameraCenter.y) {
        if (cameraRect.y >= 0) {
            this->camera->target.y -= cameraVelocityY;
        }
    }
    else {
        this->camera->target = this->camera->target;
    }

    cameraRect.x = this->camera->target.x - cameraRect.width / 2;
    cameraRect.y = this->camera->target.y - cameraRect.height / 2;

    cameraCenter.x = cameraRect.x + cameraRect.width / 2;
    cameraCenter.y = cameraRect.y + cameraRect.height / 2;
}

// Unfortunate I have to use these stupid interface functions, but it is what it is
void SceneCamera::cameraBegin() const {
    BeginMode2D(*camera);
}

void SceneCamera::cameraEnd() const {
    EndMode2D();
}

Rectangle SceneCamera::getCameraRect() const {
    const Rectangle output = cameraRect;
    return output;
}

Vector2 SceneCamera::getCameraCenter() const {
    Vector2 output = {
        this->camera->target.x + this->camera->offset.x,
        this->camera->target.y + this->camera->offset.y
    };

    return output;
}

Vector2 SceneCamera::getCameraTarget() const {
    return this->camera->target;
}

void SceneCamera::drawCameraDebugRect() const {
    DrawRectangleLines(
        cameraRect.x + 2,
        cameraRect.y + 2,
        cameraRect.width - 4,
        cameraRect.height - 4,
        RED);
}

void SceneCamera::drawDebugCrosshair() const {
    DrawLine(
        cameraCenter.x,
        cameraRect.y,
        cameraCenter.x,
        cameraCenter.y + cameraRect.height,
        RED);

    DrawLine(
        cameraRect.x,
        cameraCenter.y,
        cameraCenter.x + cameraRect.width,
        cameraCenter.y,
        RED);
}

std::shared_ptr<Camera2D> SceneCamera::getCameraPtr() const {
    return camera;
}
