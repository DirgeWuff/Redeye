//
// Created by DirgeWuff on 1/23/2026.
//

#include <filesystem>
#include "../external_libs/Toml/toml.hpp"
#include "AnimationLoader.h"
#include "../Utility/Utils.h"

namespace fs = std::filesystem;

[[nodiscard]] std::vector<animationDescriptor> loadAnimations(const std::string& dirPath) {
    if (!fs::exists(dirPath)) {
        logFatal(std::string("Cannot load animation file: " + dirPath));
        return{};
    }

    try {
        const toml::table root = toml::parse_file(dirPath);
        const auto* arr = root["animation_descriptor"].as_array();
        std::vector<animationDescriptor> animations{};

        if (!arr) {
            logFatal("arr == nullptr. loadAnimations(Args...)");
            return{};
        }

        for (const toml::node& desc : *arr) {
            const auto* tbl = desc.as_table();

            if (!tbl) {
                logFatal("tbl == nullptr. loadAnimations(Args...)");
                return{};
            }

            animationDescriptor d{};

            const Vector2 s = getVecFromToml(*tbl, "start");
            d.start = {static_cast<std::size_t>(s.x), static_cast<std::size_t>(s.y)};

            const Vector2 e = getVecFromToml(*tbl, "end");
            d.end = {static_cast<std::size_t>(e.x), static_cast<std::size_t>(e.y)};

            d.frameDuration = getValFromToml<float>(*tbl, "duration");
            d.spriteRes = getVecFromToml(*tbl, "spriteRes");

            d.type = toEnum<playbackType>(getValFromToml<std::uint8_t>(*tbl, "playbackMode")).value();
            d.id = toEnum<animationId>(getValFromToml<std::uint8_t>(*tbl, "id")).value();

            animations.push_back(d);
        }

        return animations;
    }
    catch (const toml::parse_error& e) {
        logFatal(std::string("Cannot parse animation file: ") + std::string(e.what()));
        return{};
    }
}
