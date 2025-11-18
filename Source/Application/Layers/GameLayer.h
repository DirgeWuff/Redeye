//
// Created by DirgeWuff on 5/14/25.
//

#ifndef GAMELAYER_H
#define GAMELAYER_H

#define GLSL_VERSION 330

#include <memory>
#include <filesystem>
#include "../../Core/Camera/Camera.h"
#include "../../Core/Event/EventCollider.h"
#include "../../Core/Event/EventDispatcher.h"
#include "../../Core/Entity/Player.h"
#include "../../Core/Renderer/Tilemap.h"
#include "../../Core/Backend/Layer.h"
#include "../../Core/Backend/LayerManager.h"
#include "../../Core/UI/UI.h"
#include "../../Core/Serialization/Save.h"

namespace fs = std::filesystem;

class GameLayer final : public Layer {
    MapData m_map{};
    b2WorldDef m_worldDef{};
    SceneCamera m_camera{};
    Music m_backgroundNoise{};
    saveData m_currentSave{};
    RenderTexture2D m_frameBuffer{};
    Shader m_fragShader{};
    std::shared_ptr<Player> m_playerCharacter{};
    EventDispatcher<playerContactEvent> m_collisionEventDispatcher{};
    b2WorldId m_worldId{};
    int m_flashlightPosLoc{};
    int m_flashlightDirLoc{};
    int m_screenResLoc{};
    Vector2 m_beamPosition{};
    Vector2 m_beamAngle{};
public:
    template<typename P>
    GameLayer(
        P&& playerSpritePath,
        const saveData& save) :
            m_worldDef(b2DefaultWorldDef())
{
        m_type = layerType::PRIMARY_LAYER;
        m_isEnabled = true;
        m_worldDef.gravity = {0.0f, 50.0f};
        m_worldId = b2CreateWorld(&m_worldDef);
        m_map = loadMap(save.currentMapPath.string(), m_worldId);
        m_currentSave.currentMapPath = fs::path(save.currentMapPath);
        m_currentSave.centerPosition = save.centerPosition;
        m_frameBuffer = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        m_camera = SceneCamera(m_map, 1.5f);
        m_collisionEventDispatcher = EventDispatcher<playerContactEvent>();
        m_fragShader = LoadShader(NULL, "../assets/Shaders/lighting.fsh");

        if (!IsShaderValid(m_fragShader)) {
            logErr("Unable to load shader. GameLayer::GameLayer()");
        }

        m_flashlightPosLoc = GetShaderLocation(m_fragShader, "lightPos");
        m_flashlightDirLoc = GetShaderLocation(m_fragShader, "lightDirection");
        m_screenResLoc = GetShaderLocation(m_fragShader, "screenResolution");

        const Vector2 screenRes = {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

        SetShaderValue(
            m_fragShader,
            m_screenResLoc,
            &screenRes,
            SHADER_UNIFORM_VEC2);

        try {
            // Has to be ptr. b2Body enters undefined state when moved.
            m_playerCharacter = std::make_shared<Player>(
                metersToPixels(m_currentSave.centerPosition.x),
                metersToPixels(m_currentSave.centerPosition.y),
                m_worldId,
                std::forward<P>(playerSpritePath));
        }
        catch (std::bad_alloc& e) {
            logErr(std::string("GameLayer::GameLayer(Args...) failed: ") + e.what());
            return;
        }
        catch (...) {
            logErr("GameLayer::GameLayer(Args...) failed: An unknown error has occurred.");
            return;
        }

    // Subscribe footpaw sensor.
    m_collisionEventDispatcher.subscribe(
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
    m_collisionEventDispatcher.subscribe(
        "MurderBox",
        [](const std::string& id) {
        return id.find("MurderBox") != std::string::npos;
        },
        [this](const playerContactEvent& e) {
        if (e.contactBegan) {
            m_playerCharacter->murder();
        }
    });

    // Subscribe checkpoint colliders.
    m_collisionEventDispatcher.subscribe(
        "Checkpoint",
        [](const std::string& id) {
            return id.find("Checkpoint") != std::string::npos;
        },
        [this](const playerContactEvent& e) {
            if (e.contactBegan) {
                m_currentSave.centerPosition = m_playerCharacter->getPositionCenterMeters();
                m_currentSave.currentMapPath = m_map.fullMapPath;

                saveGame(m_currentSave);

                if (LayerManager::getInstance().stackContains(layerKey::CHECKPOINT_ALERT)) {
                    LayerManager::getInstance().resumeLayer(layerKey::CHECKPOINT_ALERT);
                }
                else {
                    try {
                        LayerManager::getInstance().pushLayer(
                        layerKey::CHECKPOINT_ALERT,
                        std::make_unique<TextAlert>("checkpoint reached", 5.0f));
                    }
                    catch (const std::bad_alloc& e) {
                        logErr(std::string("Checkpoint collider failed: ") + std::string(e.what()));
                        return;
                    }
                    catch (...) {
                        logErr("Checkpoint collider failed due to an unknown error.");
                        return;
                    }
                }

                const auto* info = static_cast<sensorInfo*>(b2Shape_GetUserData(e.visitorShape));
                m_collisionEventDispatcher.unsubscribe(info->typeId);
                disableEventCollider(m_map, info->typeId);
            }
        });

    m_camera.setTarget(*m_playerCharacter);
    m_backgroundNoise = LoadMusicStream(m_map.bgNoisePath.string().c_str());
    if (!IsMusicValid(m_backgroundNoise)) {
        logErr(std::string("Cannot load background noise: " + m_map.bgNoisePath.string()));
        return;
    }

    SetMusicVolume(m_backgroundNoise, 0.30f);
    PlayMusicStream(m_backgroundNoise);
}

    ~GameLayer() override;

    void updateBeam();
    void pollEvents() override;
    void update() override;
    void draw() override;

    void destroy() override;
};

#endif // GAMELAYER_H
