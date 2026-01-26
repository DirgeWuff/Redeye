//
// Created by DirgeWuff on 1/21/2026.
//

#ifndef ANIMATIONLOADER_H
#define ANIMATIONLOADER_H

#include <vector>
#include <string>
#include "../Renderer/Animation.h"

[[nodiscard]] std::vector<animationDescriptor> loadAnimations(const std::string& dirPath);

#endif //ANIMATIONLOADER_H
