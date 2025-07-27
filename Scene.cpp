//
// Created by DirgeWuff on 5/14/25.
//

#include <box2d/box2d.h>
#include "Scene.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

constexpr float WORLD_STEP = 1.0f / 60.0f;
constexpr uint8_t SUB_STEP = 4;
CodeClock timer = CodeClock();

Scene::Scene(
    std::string&& playerSpritePath,
    std::string&& mapFilePath) :
m_worldDef(b2DefaultWorldDef()),
m_playerInput(InputHandler())
{
    m_worldDef.gravity = {0.0f, 50.0f};
    m_worldId = b2CreateWorld(&m_worldDef);

    try {
        m_playerCharacter = std::make_shared<Player>(
            200.0f,
            600.0f,
            m_worldId,
            playerSpritePath);

        m_map = std::make_unique<TiledMap>(mapFilePath.data(), m_worldId);
        m_camera = std::make_unique<SceneCamera>(*m_map);
    }
    catch (std::bad_alloc) {
        logErr(
            "Constructor init failed, std::bad_alloc thrown. Ln 44, Scene.cpp");
        return;
    }
    catch (...) {
        logErr(
            "Constructor init failed, an unknown error has occurred. Ln 49, Scene.cpp");
        return;
    }
    m_camera->setTarget(m_playerCharacter);
}

Scene::~Scene() = default;

void Scene::handleSensorEvents() const {
    const b2SensorEvents sensorContactEvents = b2World_GetSensorEvents(m_worldId);

    for (std::size_t i = 0; i < sensorContactEvents.beginCount; i++) {
        const b2SensorBeginTouchEvent& event = sensorContactEvents.beginEvents[i];
        if (isShapeIdEqual(event.sensorShapeId, m_playerCharacter->getFootpawSenorId())) {
            m_playerCharacter->setFootpawStatus(true);
        }
    }

    for (std::size_t i = 0; i < sensorContactEvents.endCount; i++) {
        const b2SensorEndTouchEvent& event = sensorContactEvents.endEvents[i];
        if (isShapeIdEqual(event.sensorShapeId, m_playerCharacter->getFootpawSenorId())) {
            m_playerCharacter->setFootpawStatus(false);
        }
    }
}

void Scene::updateScene() {
    m_playerInput.handleInput(m_playerCharacter);
    b2World_Step(m_worldId, WORLD_STEP, SUB_STEP);
    this->handleSensorEvents();
    m_playerCharacter->update();
    m_camera->update(m_playerCharacter);
}

void Scene::drawScene() const {
    m_camera->cameraBegin();

    ClearBackground(BLACK); // Might be unnecessary???
    timer.begin();
    m_map->draw(*m_camera, {0.0f, 0.0f}, WHITE);
    timer.end();
    m_playerCharacter->draw();

    // A lot of this debugging stuff is in here because it's the only place
    // an instance of Player and SceneCamera exist together
    #ifdef DEBUG_DRAW_CAMERA_CROSSHAIR
        drawDebugCameraCrosshair(m_camera);
    #endif

    #ifdef DEBUG_DRAW_PLAYER_POSITION
        drawDebugPlayerPosition(m_playerCharacter, m_camera);
    #endif

    #ifdef DEBUG_DRAW_CAMERA_RECT
        drawDebugCameraRect(m_camera);
    #endif

    #ifdef DEBUG_DRAW_PLAYER_SENSOR_STATUS
        drawDebugFootpawSensorStatus(m_playerCharacter, m_camera);
    #endif

    #ifdef DEBUG_DRAW_TERRAIN_SHAPES
        drawDebugCollisionShapes(m_map);
    #endif

    m_camera->cameraEnd();
}