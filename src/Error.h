//
// Created by DirgeWuff on 6/5/25.
//

#ifndef ERROR_H
#define ERROR_H
#include <string>

std::string getTime();                      // Get current system time and return it as an std::string
void logErr(std::string&& errorMessage);    // Halt game and log an error to the screen and an error log. Should be used for fatal errors.

#endif //ERROR_H