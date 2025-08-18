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
    b2WorldDef m_worldDef{};                                                            // b2WorldDef for the scene
    b2WorldId m_worldId{};                                                              // b2WorldId for the scene
    bodyConfig playerCfg{};                                                             // bodyConfig for the player
    std::shared_ptr<Player> m_playerCharacter{};                                        // Ptr to the player character
    InputHandler m_playerInput{};
    std::unique_ptr<EventDispatcher<playerContactEvent>> m_collisionEventDispatcher;    // Ptr to the EventDispatcher
    std::unique_ptr<TiledMap> m_map;                                                    // Ptr to the TiledMap
    std::unique_ptr<SceneCamera> m_camera;                                              // Ptr to the SceneCamera
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
