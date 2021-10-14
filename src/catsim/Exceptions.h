//
// Created by isura on 10/13/2021.
//

#ifndef CAT_SIMULATOR_CPP_EXCEPTIONS_H
#define CAT_SIMULATOR_CPP_EXCEPTIONS_H

#include <exception>
#include <string>

class ArgumentException : public std::exception {
    const std::string _message;

public:
    ArgumentException(const std::string &arg_name, const std::string &message);
    [[nodiscard]] const char *what() const noexcept override;
};

class InvalidSimulationParamsException : public std::exception {
    const std::string message;

public:
    InvalidSimulationParamsException(std::string message);
    [[nodiscard]] const char *what() const noexcept override;
};


#endif //CAT_SIMULATOR_CPP_EXCEPTIONS_H
