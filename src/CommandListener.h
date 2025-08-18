//
// Created by DirgeWuff on 5/9/25.
//

#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

class CommandListener {
public:
    CommandListener() = default;
    virtual ~CommandListener() = default;
    virtual void moveRight() = 0;
    virtual void moveLeft() = 0;
    virtual void jump() = 0;
    virtual void moveNowhere() = 0;
};

#endif //COMMAND_HANDLER_H
