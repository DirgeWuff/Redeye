//
// Created by DirgeWuff on 5/14/25.
//

#include <cstdint>
#include "raylib.h"
#include "box2d/box2d.h"
#include "Scene.h"
#include "Error.h"
#include "EventCollider.h"
#include "Utils.h"
#include "Debug.h"

constexpr float g_worldStep = 1.0f / 60.0f;
constexpr uint8_t g_subStep = 4;
auto g_timer = CodeClock();

Scene::Scene(
    std::string&& playerSpritePath,
    std::string&& mapFilePath) :
m_worldDef(b2DefaultWorldDef()),
m_uiHandler(UIHandler()),
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

    // Subscribe footpaw sensor.
    m_collisionEventDispatcher->subscribe(
        "pawbs",
        [](const std::string& id) {
            return id.find("pawbs") != std::string::npos;
        },
        [this](const playerContactEvent& e) {
            if (e.contactBegan) {
                m_playerCharacter->addContactEvent();
            }
            else {
                m_playerCharacter->removeContactEvent();
            }
        });

    // Subscribe death colliders.
    m_collisionEventDispatcher->subscribe(
        "MurderBox",
        [](const std::string& id) {
        return id.find("MurderBox") != std::string::npos;
        },
        [this](const playerContactEvent& e) {
        if (e.contactBegan) {
            m_playerCharacter->murder(m_camera);
        }
    });

    // Subscribe checkpoint colliders.
    m_collisionEventDispatcher->subscribe(
        "Checkpoint",
        [](const std::string& id) {
            return id.find("Checkpoint") != std::string::npos;
        },
        [this](const playerContactEvent& e) {
            if (e.contactBegan) {
                m_playerCharacter->setCurrentCheckpoint();

                const auto* info = static_cast<sensorInfo*>(b2Shape_GetUserData(e.visitorShape));
                m_collisionEventDispatcher->unsubscribe(info->typeId);
                m_map->disableEventCollider(info->typeId);
                m_uiHandler.addElement(std::make_unique<TextAlert>("Checkpoint reached...", 2.0f));
            }
        });
}

Scene::~Scene() = default;

void Scene::handleSensorEvents() const {
    const b2SensorEvents sensorContactEvents = b2World_GetSensorEvents(m_worldId);

    // Process all b2SensorBeginTouch events for the step.
    for (std::size_t i = 0; i < sensorContactEvents.beginCount; i++) {
        const auto&[sensorShapeId, visitorShapeId] = sensorContactEvents.beginEvents[i];
        auto* userData = static_cast<sensorInfo*>(b2Shape_GetUserData(sensorShapeId));
        if (userData) {
            m_collisionEventDispatcher->dispatch(
                userData->typeId,
                playerContactEvent{true, sensorShapeId});
        }
        else {
            logErr("Unhandled b2BeginContactEvent. sensorShapeId.userData blank.");
            break;
        }
    }

    // Process all the b2SensorEndTouch events for the step.
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

// Update our scene.
void Scene::updateScene() {
    m_uiHandler.updateUI();
    m_playerInput.handleInput(m_playerCharacter);
    b2World_Step(m_worldId, g_worldStep, g_subStep);
    handleSensorEvents();
    m_playerCharacter->update();
    m_camera->update(m_playerCharacter);
}

// Draw our scene.
void Scene::drawScene() const {
    m_camera->cameraBegin();

    ClearBackground(BLACK); // Might be unnecessary???
    g_timer.begin();
    m_map->draw(*m_camera, {0.0f, 0.0f}, WHITE);
    g_timer.end();
    m_playerCharacter->draw();

    // TODO: Move these calls to their relevant classes and use generic raw pointers
    #ifdef DEBUG
        drawDebugBodyShapes(m_playerCharacter);
        drawDebugCollisionShapes(m_map);
        drawDebugBodyCenter(m_playerCharacter);
        drawDebugCollisionVerts(m_map);
        drawDebugCameraCrosshair(m_camera);
        drawDebugCameraRect(m_camera);
        drawDebugEventColliders(m_map);
    #endif

    m_camera->cameraEnd();

    // Draw these outside of camera context!
    m_uiHandler.drawUI();

    #ifdef DEBUG
        drawControlsWindow();
        drawDebugFootpawSensorStatus(m_playerCharacter);
        drawDebugPlayerPosition(m_playerCharacter);
    #endif

}