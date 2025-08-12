//
// Created by DirgeWuff on 5/14/25.
//

#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include "Camera.h"
#include "EventCollider.h"
#include "EventDispatcher.h"
#include "InputHandler.h"
#include "Player.h"
#include "Tilemap.h"

class Scene {
    b2WorldDef m_worldDef{};
    b2WorldId m_worldId{};
    bodyConfig playerCfg{};
    std::shared_ptr<Player> m_playerCharacter{}; // Maybe change to std::unique_ptr
    InputHandler m_playerInput{};
    std::unique_ptr<EventDispatcher<playerContactEvent>> m_collisionEventDispatcher;
    std::unique_ptr<TiledMap> m_map;
    std::unique_ptr<SceneCamera> m_camera;
public:
    Scene(
        std::string&& playerSpritePath,
        std::string&& mapFilePath);

    ~Scene();

    void handleSensorEvents() const;
    void updateScene();
    void drawScene() const;
};

#endif //SCENE_H
