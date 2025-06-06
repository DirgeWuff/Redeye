//
// Created by DirgeWuff on 6/5/25.
//

// Some basic error handling/logging utils

#include <fstream>
#include <chrono>
#include <sstream>
#include "raylib.h"

std::string getTime() {
    const auto currentTime = std::chrono::system_clock::now();
    const std::time_t currentTimeType = std::chrono::system_clock::to_time_t(currentTime);
    const std::tm tm = *std::localtime(&currentTimeType);

    std::ostringstream stream;
    stream << std::put_time(&tm, "%m-%d-%Y %H:%M:%S");

    return stream.str();
}

void logErr(std::string&& errorMessage) {
    SetExitKey(KEY_ESCAPE);
    std::string msg = "FATAL ERROR: " + errorMessage;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(msg.c_str(), 5, 5, 15, RED);
        DrawText("Press Esc to exit.", 5, 20, 15, RED);
        EndDrawing();
    }

    if (!std::filesystem::exists("ErrorLog.txt")) {
        std::ofstream f("ErrorLog.txt");

        if (f.is_open()) {
            f << getTime() <<  " " << "FATAL ERROR: " << errorMessage << std::endl;
            f.close();
        }
    }
    else {
        std::ofstream f;

        f.open("ErrorLog.txt", std::ios::app); // Open in append mode
        f << getTime() << " " << errorMessage << std::endl;
        f.close();
    }
}