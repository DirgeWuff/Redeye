//
// Created by DirgeWuff on 5/13/25.
//

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <memory>
#include "CommandListener.h"


class InputHandler {
public:
    InputHandler();
    virtual ~InputHandler();
    void handleInput(const std::shared_ptr<CommandListener>& listener);
};



#endif //INPUTHANDLER_H
