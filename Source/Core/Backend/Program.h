//
// Author: DirgeWuff
// Created on: 10/1/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Core wrapper class for the entire program. Responsible for initializing
// the rendering API, calling startup routines, and executing the main game
// loop, as well as terminating the program. This class is a singleton and
// is called from main.cpp.


#ifndef APPLICATION_H
#define APPLICATION_H

namespace RE::Core {
    class Program {
        static Program m_appInstance;
        bool m_gameRunning;

        Program();
        ~Program() = default;

        Program(Program&) = delete;
        Program(Program&&) = delete;
        Program& operator=(Program&) = delete;
        Program&& operator=(Program&&) = delete;
    public:
        void init();
        void run() const;

        static Program& getInstance() noexcept;
        void shutdown() noexcept;
    };
}


#endif //APPLICATION_H
