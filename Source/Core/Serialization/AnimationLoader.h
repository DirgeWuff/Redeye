//
// Author: DirgeWuff
// Created on: 1/23/26
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Declaration of a function to load an std::vector<animationDescriptor> from
// TOML file. Designed to eliminate hard-coding of animationDescriptors.

#ifndef ANIMATIONLOADER_H
#define ANIMATIONLOADER_H

#include <vector>
#include <string>
#include "../Renderer/Animation.h"

namespace RE::Core {
    [[nodiscard]] std::vector<animationDescriptor> loadAnimations(const std::string& dirPath);
}

#endif //ANIMATIONLOADER_H
