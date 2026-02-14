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
// Declaration of a functions to load an std::vector<std::unique_ptr<animationDescriptor>> from
// TOML file. Designed to eliminate hard-coding of animationDescriptors.

#ifndef ANIMATIONLOADER_H
#define ANIMATIONLOADER_H

#include <vector>
#include <string>
#include <memory>
#include "../external_libs/Toml/toml.hpp"
#include "../Renderer/Animation.h"

namespace RE::Core {
    [[nodiscard]] animationDescriptor parseDescriptorBase(const toml::table& tbl);
    [[nodiscard]] std::vector<std::unique_ptr<animationDescriptor>> loadAnimations(const std::string& dirPath);
}

#endif //ANIMATIONLOADER_H
