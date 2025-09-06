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
    MapData m_map{};                                                                    // Current map in the scene
    bool m_mapLoading{};                                                                // Are we currently loading a map?
    // Consider making a sound handler class later on
    Music m_backgroundNoise{};                                                          // Background noise in the scene
    std::shared_ptr<Player> m_playerCharacter{};                                        // Ptr to the player character
    std::unique_ptr<EventDispatcher<playerContactEvent>> m_collisionEventDispatcher;    // Ptr to the EventDispatcher
    std::unique_ptr<SceneCamera> m_camera;                                              // Ptr to the SceneCamera
    UIHandler m_uiHandler{};                                                            // Handler for the user interface
    InputHandler m_playerInput{};                                                       // Input handler for the player character
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
