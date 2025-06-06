//
// Created by DirgeWuff on 5/14/25.
//

#include "Scene.h"
#include "Error.h"

Scene::Scene(
    std::string&& playerSpritePath,
    std::string&& mapFilePath) :
playerInput(InputHandler()),
map(TiledMap(mapFilePath.data())),
camera(SceneCamera(map)),
gravity(3.0f),
groundPos(510.0f)
{
    try {
        playerCharacter = std::make_shared<Player>(playerSpritePath);
    }
    catch (std::bad_alloc) {
        logErr("Constructor init failed: Scene::Scene. std::bad_alloc thrown. Ln 19, Scene.cpp");
    }

    camera.setTarget(playerCharacter);
}

Scene::~Scene() = default;

void Scene::updateScene() {
    playerInput.handleInput(playerCharacter);
    this->playerCharacter->update();
    camera.update(playerCharacter);
}

void Scene::drawScene() {
    camera.cameraBegin();
    ClearBackground(BLACK); // Might be unnecessary???
    map.drawMap(camera, {0.0f, 0.0f}, WHITE);
    this->playerCharacter->draw();
    camera.drawDebugCrosshair();
    camera.cameraEnd();
}
