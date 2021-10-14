//
// Created by isura on 10/13/2021.
//

#include "Logger.h"

#include <ctime>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "Time.h"

LoggerHelper::LoggerHelper(LogMethod method, const std::string &file)
        : method(method) {
    if (this->method == LogMethod::none) {
#ifdef _WIN32
        this->file = new std::ofstream("nul");
#else
        this->file = new std::ofstream("/dev/null");
#endif
        this->dynamic = true;
    } else if (this->method == LogMethod::console) {
        this->file = &std::cout;
        this->dynamic = false;
    } else if (this->method == LogMethod::file) {
        this->file = new std::ofstream(file);
        this->dynamic = true;
    }
}

LoggerHelper::~LoggerHelper() {
    if (this->dynamic) {
        auto *ofs = dynamic_cast<std::ofstream *>(this->file);
        if (ofs->is_open()) {
            ofs->close();
        }
        delete this->file;
    }
}

LoggerHelper::LoggerHelper(LoggerHelper &&other) noexcept
        : method(other.method), file(other.file), dynamic(other.file) {
    other.file = nullptr;
    other.dynamic = false;
}

LoggerHelper &LoggerHelper::operator=(LoggerHelper other) {
    std::swap(file, other.file);
    std::swap(method, other.method);
    std::swap(dynamic, other.dynamic);
    return *this;
}

LoggerHelper Logger::helper(LogMethod::console, "");
bool Logger::first_time = true;

void Logger::setup(LogMethod method, const std::string &file) {
    Logger::helper = LoggerHelper(method, file);
    Logger::first_time = true;
}

LoggerHelper::type &Logger::log() {
    char buffer[30];
    long millis;
    struct tm *tm_info;
    struct timeval tv{};

    get_time_of_day(&tv, nullptr);
    millis = std::lrint(static_cast<double>(tv.tv_usec) / 1000.0); // Round to nearest millis
    if (millis >= 1000) { // Allow for rounding up to the nearest second
        millis -= 1000;
        tv.tv_sec++;
    }

    std::time_t t = tv.tv_sec;
    tm_info = std::localtime(&t);

    std::strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", tm_info);

    std::ostringstream oss;
    oss << buffer << '.' << std::setfill('0') << std::setw(3) << millis;

    if(!Logger::first_time) {
        *Logger::helper.file << '\n';
    }
    Logger::first_time = false;
    return *Logger::helper.file << oss.str() << ": ";
}

void Logger::sep() {
    Logger::log() << "------------";
}
