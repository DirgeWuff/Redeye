//
// Created by DirgeWuff on 5/14/25.
//

#include "Scene.h"
Scene::Scene(
    std::string&& playerSpritePath,
    std::string&& mapFilePath) :
gravity(3.0f),
groundPos(510.0f)
{
    playerCharacter = std::make_shared<Player>(playerSpritePath);
    playerInput = InputHandler();
    map = TiledMap(mapFilePath.data());
    camera = SceneCamera(map);
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
