//
// Created by DirgeWuff on 5/14/25.
//

#include <cstdint>
#include "external_libs/Raylib/include/raylib.h"
#include "Scene.h"
#include "Error.h"
#include "Debug.h"
#include "EventCollider.h"
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
            390.0f,
            m_worldId,
            playerSpritePath);

        m_map = std::make_unique<TiledMap>(mapFilePath.data(), m_worldId);
        m_camera = std::make_unique<SceneCamera>(*m_map, 2.0f);
        m_collisionEventDispatcher = std::make_unique<EventDispatcher<playerContactEvent>>();
    }
    catch (std::bad_alloc) { // NEVER EVER had this throw but worth having here
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

    m_collisionEventDispatcher->subscribe(
        "pawbs",
        [this](const playerContactEvent e) {
            if (e.contactBegan) {
                m_playerCharacter->addContactEvent();
            }
            else {
                m_playerCharacter->removeContactEvent();
            }
        });

    m_collisionEventDispatcher->subscribe(
        "MurderBox",
        [this](const playerContactEvent& e) {
            if (e.contactBegan) {
                m_playerCharacter->murder(m_camera);
            }
        });

    m_collisionEventDispatcher->subscribe(
        "GenericCollider",
        std::move([](const playerContactEvent& e) {
            if (e.contactBegan) {
                std::cout << "Generic collider struck" << std::endl;
            }
        }));
}

Scene::~Scene() = default;

void Scene::handleSensorEvents() const {
    const b2SensorEvents sensorContactEvents = b2World_GetSensorEvents(m_worldId);

    for (std::size_t i = 0; i < sensorContactEvents.beginCount; i++) {
        const auto&[sensorShapeId, visitorShapeId] = sensorContactEvents.beginEvents[i];
        auto* userData = static_cast<sensorInfo*>(b2Shape_GetUserData(sensorShapeId));
        if (userData) {
            m_collisionEventDispatcher->dispatch(
                userData->typeId,
                playerContactEvent{true, sensorShapeId});
        }
        else {
            logErr("Unhandled b2BeginContactEvent. sensorShapeId.userData bank.");
            break;
        }
    }

    for (std::size_t i = 0; i < sensorContactEvents.endCount; i++) {
        const auto&[sensorShapeId, visitorShapeId] = sensorContactEvents.endEvents[i];
        auto* userData = static_cast<sensorInfo*>(b2Shape_GetUserData(sensorShapeId));
        if (userData) {
            m_collisionEventDispatcher->dispatch(
                userData->typeId,
                playerContactEvent{false, sensorShapeId});
        }
        else {
            logErr("Unhandled b2EndContactEvent. senorShapeId.userData blank.");
            break;
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
    #ifdef DEBUG
        drawDebugBodyShapes(m_playerCharacter);
        drawDebugCollisionShapes(m_map);
        drawDebugBodyCenter(m_playerCharacter);
        drawDebugCollisionVerts(m_map);
        drawDebugCameraCrosshair(m_camera);
        drawDebugCameraRect(m_camera);
    #endif

    m_camera->cameraEnd();

    #ifdef DEBUG
        drawControlsWindow();
        drawDebugFootpawSensorStatus(m_playerCharacter);
        drawDebugPlayerPosition(m_playerCharacter);
    #endif

}