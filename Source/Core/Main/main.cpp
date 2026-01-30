//
// Author: DirgeWuff
// Created on: Date lost/unknown (Project creation date)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Main entry point for the program

#include "../Backend/Application.h"

int main() {

    Application::getInstance().init();
    Application::getInstance().run();

    return 0;
}