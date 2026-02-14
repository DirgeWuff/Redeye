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
// Function definition of the animation loader.

#include <filesystem>
#include "AnimationLoader.h"
#include "../Renderer/Animation.h"
#include "../Utility/Utils.h"

namespace fs = std::filesystem;

namespace RE::Core {
    [[nodiscard]] animationDescriptor parseDescriptorBase(const toml::table &tbl) {
        const Vector2 s = getVecFromToml(tbl, "start");
        const Vector2 e = getVecFromToml(tbl, "end");

        return {
            spriteIndex{static_cast<std::size_t>(s.x), static_cast<std::size_t>(s.y)},
            spriteIndex{static_cast<std::size_t>(e.x), static_cast<std::size_t>(e.y)},
            getVecFromToml(tbl, "spriteRes"),
            getValFromToml<float>(tbl, "duration"),
            toEnum<animPlaybackMode>(getValFromToml<std::uint8_t>(tbl, "playbackMode")).value(),
            toEnum<animationId>(getValFromToml<std::uint8_t>(tbl, "id")).value(),
            toEnum<animType>(getValFromToml<std::uint8_t>(tbl, "type")).value()
        };
    }

    [[nodiscard]] std::vector<std::unique_ptr<animationDescriptor>> loadAnimations(const std::string& dirPath) {
        if (!fs::exists(dirPath)) {
            logFatal(std::string("Cannot load animation file: " + dirPath));
            return{};
        }

        try {
            const toml::table root = toml::parse_file(dirPath);
            const auto* arr = root["animation_descriptor"].as_array();
            std::vector<std::unique_ptr<animationDescriptor>> animations{};

            if (!arr) {
                logFatal("arr == nullptr. loadAnimations(Args...)");
                return{};
            }

            for (const auto& desc : *arr) {
                const auto* tbl = desc.as_table(); // NOLINT

                if (!tbl) {
                    logFatal("tbl == nullptr. loadAnimations(Args...)");
                    return{};
                }

                const animType type = toEnum<animType>(getValFromToml<std::uint8_t>(*tbl, "type")).value();

                switch (type) {
                    case animType::SILENT: {
                        auto d = std::make_unique<animationDescriptor>(parseDescriptorBase(*tbl));
                        animations.push_back(std::move(d));

                        break;
                    }
                    case animType::KEYFRAME_SOUND: {

                        auto d = std::make_unique<keyframeSoundDescriptor>(
                            parseDescriptorBase(*tbl),
                            getArrFromToml<std::uint8_t>(*tbl, "soundFrames"),
                            toEnum<soundId>(getValFromToml<std::uint8_t>(*tbl, "soundFrameSoundId")).value());

                        animations.push_back(std::move(d));

                        break;
                    }
                    case animType::TRANSITION_SOUND: {
                        auto d = std::make_unique<transitionSoundDescriptor>(
                            parseDescriptorBase(*tbl),
                            toEnum<soundId>(getValFromToml<std::uint8_t>(*tbl, "transitionSound")).value());

                        animations.push_back(std::move(d));

                        break;
                    }
                    default: {
                        logFatal("No such animation type!");
                        break;
                    }
                }
            }

            return animations;
        }
        catch (const toml::parse_error& e) {
            logFatal(std::string("Cannot parse animation file: ") + std::string(e.what()));
            return{};
        }
        catch (...) {
            logFatal("Failed to load animation file, an unknown error has occurred.");
            return{};
        }
    }
}