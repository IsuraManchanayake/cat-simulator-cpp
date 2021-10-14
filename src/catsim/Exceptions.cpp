//
// Created by isura on 10/13/2021.
//

#include "Exceptions.h"

ArgumentException::ArgumentException(const std::string &arg_name, const std::string &message)
        : _message(arg_name + ": " + message) {}

const char *ArgumentException::what() const noexcept {
    return this->_message.c_str();
}

InvalidSimulationParamsException::InvalidSimulationParamsException(std::string message)
        : message(std::move(message)) {}

const char *InvalidSimulationParamsException::what() const noexcept {
    return this->message.c_str();
}

