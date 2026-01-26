//
// Created by DirgeWuff on 10/19/2025.
//

#include <filesystem>
#include <string>
#include "../external_libs/Toml/toml.hpp"
#include "../Utility/Logging.h"
#include "../Utility/Utils.h"
#include "../Utility/Globals.h"
#include "Save.h"

namespace fs = std::filesystem;

// TODO: Add data/code to save and load the player's direction and flashlight beam angle

void createDefaultSave() {
    if (!fs::exists(g_saveFolderPath))
        fs::create_directory(g_saveFolderPath);

    std::ofstream f(g_saveFilePath);

    // TODO: Figure out how to dynamically load playerX/Y from starting map file
    constexpr std::string_view saveData = R"(
        [save]
        currentMapPath = "../assets/Map data/Version 3/TestLvl3.tmj"
        playerPosX = 64.0
        playerPosY = 315.0
           )";

    try {
        const toml::table tbl = toml::parse(saveData);

        if (f.is_open()) {
            f << tbl;
            f.close();
        }
        else {
            logFatal("Error creating default save, cannot create file: Save::createDefaultSave().");
        }
    }
    catch (const toml::parse_error& e) {
        logFatal(std::string("Error creating default save: ") + std::string(e.what()) +
            std::string(". Save::createDefaultSave"));
    }
}

void saveGame(const saveData& data) {
    // Should never happen unless the user deleted the dir...
    if (!fs::exists(g_saveFolderPath))
        fs::create_directory(g_saveFolderPath);

    // Explicitly overwrite if the file already exists...
    std::ofstream f(g_saveFilePath, std::ios::out | std::ios::trunc);

    toml::table section;
    section.insert("currentMapPath", data.currentMapPath.string());
    section.insert("playerPosX", metersToPixels(data.centerPosition.x));
    section.insert("playerPosY", metersToPixels(data.centerPosition.y));

    toml::table tbl;
    tbl.insert("save", std::move(section));

    if (f.is_open()) {
        f << tbl;
        f.close();
    }
    else {
        logFatal("Cannot save game, error opening file handle: Save::saveGame(Args...)");
    }
}

saveData loadGame() {
    // Again, should never happen in theory
    if (!fs::exists(g_saveFilePath))
        createDefaultSave();

    try {
        const toml::table tbl = toml::parse_file(g_saveFilePath);
        saveData data{};

        data.currentMapPath = getNestedValFromToml<std::string>(tbl, "save", "currentMapPath");
        data.centerPosition.x = pixelsToMeters(getNestedValFromToml<float>(tbl, "save", "playerPosX"));
        data.centerPosition.y = pixelsToMeters(getNestedValFromToml<float>(tbl, "save", "playerPosY"));

        return data;
    }
    catch (const toml::parse_error& e) {
        logFatal(std::string("Cannot parse save data: ") + std::string(e.what()) +
            std::string(". Save::loadGame()"));
        return {};
    }
}
