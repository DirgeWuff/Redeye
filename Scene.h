//
// Created by DirgeWuff on 5/14/25.
//

#ifndef SCENE_H
#define SCENE_H

#include "Camera.h"
#include "InputHandler.h"
#include "Player.h"
#include "Tilemap.h"

class Scene {
    std::shared_ptr<Player> playerCharacter;
    InputHandler playerInput;
    TiledMap map;
    SceneCamera camera;

    const float gravity;
    const float groundPos; // Temporary
public:
    Scene(
        std::string&& playerSpritePath,
        std::string&& mapFilePath);

    ~Scene();
    void updateScene();
    void drawScene();
};

#endif //SCENE_H
