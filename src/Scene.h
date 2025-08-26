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
#include "UI.h"

class Scene {
    b2WorldDef m_worldDef{};                                                            // b2WorldDef for the Scene
    b2WorldId m_worldId{};                                                              // b2WorldId for the Scene
    bodyConfig playerCfg{};                                                             // bodyConfig for the Player
    std::shared_ptr<Player> m_playerCharacter{};                                        // Ptr to the player character
    UIHandler m_uiHandler{};                                                            // Handler for the user interface
    InputHandler m_playerInput{};                                                       // Input handler for the player character
    std::unique_ptr<EventDispatcher<playerContactEvent>> m_collisionEventDispatcher;    // Ptr to the EventDispatcher
    std::unique_ptr<TiledMap> m_map;                                                    // Ptr to the TiledMap
    std::unique_ptr<SceneCamera> m_camera;                                              // Ptr to the SceneCamera
public:
    Scene(
        std::string&& playerSpritePath,
        std::string&& mapFilePath);

    ~Scene();

    void handleSensorEvents() const;    // Handle all sensor events in the world created by Box2D.
    void updateScene();                 // Update the Scene.
    void drawScene() const;             // Draw the Scene.
};

#endif //SCENE_H
