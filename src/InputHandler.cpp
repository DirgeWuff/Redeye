//
// Created by DirgeWuff on 5/13/25.
//

#include "raylib.h"
#include "InputHandler.h"

InputHandler::InputHandler() = default;

InputHandler::~InputHandler() = default;

void InputHandler::handleInput(const std::shared_ptr<CommandListener>& listener) {
    if (IsKeyDown(KEY_D)) {
        listener->moveRight();
    }
    else if (IsKeyDown(KEY_A)) {
        listener->moveLeft();
    }

    if (IsKeyDown(KEY_SPACE)) {
        listener->jump();
    }

    if (IsKeyUp(KEY_D) && IsKeyUp(KEY_A)) {
        listener->moveNowhere();
    }
}
