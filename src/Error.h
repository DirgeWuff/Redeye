//
// Created by DirgeWuff on 6/5/25.
//

#ifndef ERROR_H
#define ERROR_H
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>
#include "raylib.h"

// Get current system time and return it as a string
inline std::string getTime() {
    const auto currentTime = std::chrono::system_clock::now();
    const std::time_t currentTimeType = std::chrono::system_clock::to_time_t(currentTime);
    const std::tm tm = *std::localtime(&currentTimeType);

    std::ostringstream stream;
    stream << std::put_time(&tm, "%m-%d-%Y %H:%M:%S");

    return stream.str();
}

// Halt game and log an error to the screen and an error log. Should be used for fatal errors.
template<typename T>
void logErr(const T& errorMessage) {
    SetExitKey(KEY_ESCAPE);

    std::string msg = "FATAL ERROR: " + std::string(errorMessage);
    const char* exitMsg = "Press Esc to exit.";
    const int exitMsgPosX = (1500 - MeasureText(exitMsg, 35)) / 2;
    int messageLen = MeasureText(msg.c_str(), 35);

    while (!WindowShouldClose()) {

        // Wrap long messages
        // Give a 300 px buffer to the screen size
        if (messageLen > 1200) {
            std::string buff{};
            std::string line{};
            int currLineLen{};
            std::vector<std::string> wrappedMessage{};
            std::stringstream ss(msg);

            // Break the text up and put it in vector
            while (ss) {
                std::getline(ss, buff, ' ');
                if (currLineLen < 1200) {
                    line += buff + " ";
                    currLineLen = MeasureText(line.c_str(), 35);
                }
                else {
                    wrappedMessage.push_back(line);
                    currLineLen = 0;
                    line = "";
                }
            }

            // Draw each string in vector to screen at correct pos
            BeginDrawing();
            ClearBackground(BLACK);

            int msgPosY = 5;
            for (const auto& str : wrappedMessage) {
                int posX  = (1500 - MeasureText(str.c_str(), 35)) / 2;

                DrawText(str.c_str(), posX, msgPosY, 35, RED);

                msgPosY += 40;
            }

            DrawText(exitMsg, exitMsgPosX, msgPosY + 60, 35, RED);

            EndDrawing();
        }
        else {
            BeginDrawing();

            ClearBackground(BLACK);

            const int posX = (1500 - MeasureText(msg.c_str(), 35)) / 2;

            DrawText(msg.c_str(), posX, 5, 35, RED);
            DrawText(exitMsg, exitMsgPosX, 95, 35, RED);

            EndDrawing();
        }
    }

    // Write error message to a text log
    if (!std::filesystem::exists("ErrorLog.txt")) {
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

#endif //ERROR_H