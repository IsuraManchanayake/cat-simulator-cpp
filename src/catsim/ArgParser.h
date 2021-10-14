//
// Created by isura on 10/12/2021.
//

#ifndef CAT_SIMULATOR_CPP_ARGPARSER_H
#define CAT_SIMULATOR_CPP_ARGPARSER_H

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Enums.h"

namespace detail {
    struct BaseArg {
        std::string name;
        bool required;
        bool parsed;
        std::string help;
        std::string default_string;

//    std::variant<std::string, int, bool, double> value;
//    std::variant<std::string, int, bool, double> default_value;
//    T value;
//    T default_value;
        BaseArg(std::string name, bool required, std::string help);

        virtual ~BaseArg() = default;

        virtual void parse_value(std::vector<std::string> &raw_args) = 0;

        void parse(std::vector<std::string> &raw_args);
    };

    template<typename T>
    struct ArgHelper : BaseArg {
        T value;
        T default_value;

        ArgHelper(const std::string &name, const std::string &help, bool required = true, T default_value = {})
                : BaseArg(name, required, help), default_value(default_value) {
            std::ostringstream oss;
            oss << default_value;
            this->default_string = oss.str();
        }
    };

    template<typename T>
    struct Arg;

    template<>
    struct Arg<std::string> : public ArgHelper<std::string> {
        using ArgHelper<std::string>::ArgHelper;

        void parse_value(std::vector<std::string> &raw_args) override {
            this->name = raw_args.back();
            raw_args.pop_back();
            this->value = raw_args.back();
            raw_args.pop_back();
        }
    };

    template<>
    struct Arg<int> : public ArgHelper<int> {
        using ArgHelper<int>::ArgHelper;

        void parse_value(std::vector<std::string> &raw_args) override {
            this->name = raw_args.back();
            raw_args.pop_back();
            this->value = std::stoi(raw_args.back());  // throws
            raw_args.pop_back();
        }
    };

    template<>
    struct Arg<size_t> : public ArgHelper<size_t> {
        using ArgHelper<size_t>::ArgHelper;

        void parse_value(std::vector<std::string> &raw_args) override {
            this->name = raw_args.back();
            raw_args.pop_back();
            this->value = std::stoull(raw_args.back());  // throws
            raw_args.pop_back();
        }
    };

    template<>
    struct Arg<Neighborhood> : public ArgHelper<Neighborhood> {
        using ArgHelper<Neighborhood>::ArgHelper;

        void parse_value(std::vector<std::string> &raw_args) override {
            this->name = raw_args.back();
            raw_args.pop_back();
            this->value = raw_args.back() == "moore"
                          ? Neighborhood::Moore : Neighborhood::VonNeumann;
            raw_args.pop_back();
        }
    };

    template<>
    struct Arg<bool> : public ArgHelper<bool> {
        using ArgHelper<bool>::ArgHelper;

        void parse_value(std::vector<std::string> &raw_args) override {
            this->name = raw_args.back();
            raw_args.pop_back();
            this->value = true;
        }
    };

    template<>
    struct Arg<double> : public ArgHelper<double> {
        using ArgHelper<double>::ArgHelper;

        void parse_value(std::vector<std::string> &raw_args) override {
            this->name = raw_args.back();
            raw_args.pop_back();
            this->value = std::stod(raw_args.back());  // throws
            raw_args.pop_back();
        }
    };
}

struct ArgParser {
    std::vector<std::string> _raw_args;
    std::vector<std::string> ordered_args;
    std::map<std::string, detail::BaseArg *> _parsers;
    std::string description;

    ArgParser(int argc, const char **argv, std::string description = {});

    ~ArgParser();

    template<typename T>
    ArgParser &
    add_parser(const std::string &name, const std::string &help = "", bool required = true, T default_value = {});

    void parse();

    void display_help() const;

    template<typename T>
    const T &get_value(const std::string &name, const T &default_value = {}) const;
};

template<typename T>
ArgParser &ArgParser::add_parser(const std::string &name, const std::string &help, bool required, T default_value) {
    auto it = this->_parsers.find(name);
    if (it != this->_parsers.end()) {
        delete it->second;
        this->_parsers.erase(it);
    } else {
        ordered_args.push_back(name);
    }
    detail::BaseArg *arg = new detail::Arg<T>(name, help, required, default_value);
    this->_parsers[name] = arg;
    return *this;
}

template<typename T>
const T &ArgParser::get_value(const std::string &name, const T &default_value) const {
    auto it = this->_parsers.find(name);
    if (it == this->_parsers.end()) {
        return default_value;
    }
    const auto parser = static_cast<detail::Arg<T> *>(it->second);
    if (parser->parsed) {
        return parser->value;
    }
    return parser->default_value;
}

#endif //CAT_SIMULATOR_CPP_ARGPARSER_H
