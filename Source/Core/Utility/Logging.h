//
// Created by DirgeWuff on 6/5/25.
//

#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <iostream>
#include "raylib.h"

#include "Globals.h"

namespace fs = std::filesystem;

inline std::string g_logFilePath;

// Get current system time and return it as a string
// Wanted to move this but putting it in Utils.h makes a circular dependency I don't feel like fixing rn
inline std::string getTime() {
    const auto currentTime = std::chrono::system_clock::now();
    const std::time_t currentTimeType = std::chrono::system_clock::to_time_t(currentTime);
    const std::tm tm = *std::localtime(&currentTimeType);

    std::ostringstream stream;
    stream << std::put_time(&tm, "%m-%d-%Y %H:%M:%S");

    return stream.str();
}

// Remove illegal characters from getTime for filename purposes...
inline std::string getTimeFilename() {
    std::string fn = getTime();

    std::replace(fn.begin(), fn.end(), ' ' , '_');
    std::replace(fn.begin(), fn.end(), ':', '-');

    return fn;
}

// Create a new debug log for this session, runs once per launch of game
inline void createNewSessionLog() {
    if (!fs::exists("Logs") && !fs::is_directory("Logs"))
        fs::create_directory("Logs");

    const std::string logPath = "Logs/Debug Log " + getTimeFilename() + ".txt";

    if (!fs::exists(logPath)) { // Should always be true...
        std::ofstream f(logPath);
        if (f.is_open())
            f.close();
    }

    g_logFilePath = logPath;
}

// Halt game and log an error to the screen and an error log. Should be used for fatal errors.
template<typename T>
void logFatal(const T& errorMessage) {
    SetExitKey(KEY_ESCAPE);

    std::string msg = "FATAL ERROR: " + std::string(errorMessage);

    const char* exitMsg = "Press Esc to exit.";
    const int exitMsgPosX = (1500 - MeasureText(exitMsg, 20)) / 2;
    int messageLen = MeasureText(msg.c_str(), 20);

    while (!WindowShouldClose()) {

        // Wrap long messages
        // Give a 500 px buffer to the screen size
        if (messageLen > 1000) {
            std::string buff{};
            std::string line{};
            int currLineLen{};
            std::vector<std::string> wrappedMessage{};
            std::stringstream ss(msg);

            while (ss) {
                // Potentially very problematic.
                // Error messages often contain large portions with many spaces, and large portions with few.
                // Should really come up with a better way to split text up so it ALWAYS prints properly...
                std::getline(ss, buff, ' ');
                if (currLineLen < 1000) {
                    line += buff + " ";
                    currLineLen = MeasureText(line.c_str(), 20);
                    buff = "";
                }
                else {
                    line += buff;
                    wrappedMessage.push_back(line);
                    currLineLen = 0;
                    line = "";
                }
            }

            wrappedMessage.push_back(line);

            // Draw each string in vector to screen at correct pos
            BeginDrawing();
            ClearBackground(BLACK);

            int msgPosY = 5;
            for (const auto& str : wrappedMessage) {
                int posX  = (1500 - MeasureText(str.c_str(), 20)) / 2;

                DrawText(str.c_str(), posX, msgPosY, 20, RED);

                msgPosY += 40;
            }

            DrawText(exitMsg, exitMsgPosX, msgPosY + 60, 20, RED);

            EndDrawing();
        }
        else {
            BeginDrawing();

            ClearBackground(BLACK);

            const int posX = (1500 - MeasureText(msg.c_str(), 20)) / 2;

            DrawText(msg.c_str(), posX, 5, 20, RED);
            DrawText(exitMsg, exitMsgPosX, 95, 20, RED);

            EndDrawing();
        }
    }

    // Write error message to a text log
    if (!fs::exists("ErrorLog.txt")) {
        std::ofstream f("ErrorLog.txt");

        if (f.is_open()) {
            f << getTime() <<  " " << msg << std::endl;
            f.close();
        }
    }
    else {
        std::ofstream f;

        f.open("ErrorLog.txt", std::ios::app); // Open in append mode
        f << getTime() << " " << msg << std::endl;
        f.close();
    }

    CloseWindow();
}

template<typename... Args>
void logDbg(Args&&... args) {
    std::ofstream f(g_logFilePath, std::ios::app);
    if (!f.is_open()) return;

    const std::string time = getTime();

    std::cout << time << " DEBUG: ";
    f << time << " DEBUG: ";

    ((std::cout << std::forward<Args>(args)), ...);
    std::cout << std::endl;
    ((f << std::forward<Args>(args)), ...);
    f << std::endl;

    f.close();
}

#endif //LOGGING_H