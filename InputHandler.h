//
// Created by DirgeWuff on 5/13/25.
//

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <memory>
#include "CommandListener.h"

class InputHandler final {
public:
    InputHandler();
    ~InputHandler();
    void handleInput(const std::shared_ptr<CommandListener>& listener);
};

#endif //INPUTHANDLER_H
