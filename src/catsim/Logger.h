//
// Created by isura on 10/13/2021.
//

#ifndef CAT_SIMULATOR_CPP_LOGGER_H
#define CAT_SIMULATOR_CPP_LOGGER_H

#include "Enums.h"

#include <functional>
#include <memory>
#include <ostream>

struct LoggerHelper {
    using type = std::ostream;

    LogMethod method;
    LoggerHelper::type *file;
    bool dynamic;

    LoggerHelper(LogMethod method, const std::string &file);
    LoggerHelper(LoggerHelper &&other) noexcept ;
    LoggerHelper &operator=(LoggerHelper other);
    ~LoggerHelper();
};

struct Logger {
    static LoggerHelper helper;

    static void setup(LogMethod method, const std::string &file="simulation.log");
    static LoggerHelper::type &log();
    static void sep();

    static bool first_time;
};


#endif //CAT_SIMULATOR_CPP_LOGGER_H
