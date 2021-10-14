//
// Created by isura on 10/12/2021.
//

#include "ArgParser.h"
#include "Exceptions.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>

detail::BaseArg::BaseArg(std::string name, bool required, std::string help)
    : name(std::move(name)), required(required), help(std::move(help)), parsed(false) {}

void detail::BaseArg::parse(std::vector<std::string> &raw_args) {
    this->parse_value(raw_args);
    this->parsed = true;
}

//template<typename T>
//std::any ArgHelper<T>::get_value() const {
//    return this->value;
//}

ArgParser::ArgParser(int argc, const char **argv, std::string description)
    : description(std::move(description)) {
    for(size_t i = argc; i-- > 0;) {
        this->_raw_args.emplace_back(argv[i]);
    }
}

ArgParser::~ArgParser() {
    for(const auto& parser_pair: this->_parsers) {
        delete parser_pair.second;
    }
}

void ArgParser::parse() {
    while(!this->_raw_args.empty()) {
        const auto& name = this->_raw_args.back();
        const auto it = this->_parsers.find(name);
        if(it != this->_parsers.end()) {
            try {
                it->second->parse(this->_raw_args);
            } catch (std::exception& e) {
                throw ArgumentException(it->second->name, e.what());
            }
        } else {
            this->_raw_args.pop_back();
        }
    }
    for(const auto &name_parser_pair: this->_parsers) {
        const auto &parser = name_parser_pair.second;
        if(parser->required && !parser->parsed) {
            throw ArgumentException(parser->name, "Required but not provided.");
        }
    }
}

void ArgParser::display_help() const {
    std::vector<const detail::BaseArg *> mandatory;
    std::vector<const detail::BaseArg *> optional;

    std::ostringstream oss_command;
    oss_command << "Usage: " << "cat_simulator_cpp_run" << " ";
    for(const auto &ordered_arg : this->ordered_args) {
        const auto &name = ordered_arg;
        const auto &arg = this->_parsers.find(name)->second;

        if (!arg->required) {
            oss_command << "[";
        }
        std::string dv(arg->name.begin() + 2, arg->name.end());
        std::transform(dv.begin(), dv.end(), dv.begin(), [](auto c) { return ::toupper(c); });
        oss_command << arg->name << " " << dv;
        if (!arg->required) {
            oss_command << "]";
        }
        oss_command << " ";
        if(arg->required) {
            mandatory.push_back(arg);
        } else {
            optional.push_back(arg);
        }
    }
    std::string command = oss_command.str();
    for(int i = 0; i < command.size(); i += 120) {
        std::cerr << std::string(command.begin() + i, command.begin() + std::min(i + 120, (int) command.size())) << '\n';
    }
    
    std::cerr << '\n' << this->description << '\n';
    auto print_arg = [] (const std::vector<const detail::BaseArg *> &arg_list) -> void {
        for(const auto &arg: arg_list) {
            std::ostringstream oss_name;
            oss_name << " " << arg->name;
            std::string start_str = oss_name.str();

            std::cerr << start_str;

            size_t padding = 0;
            if(start_str.size() > 23) {
                std::cerr << '\n';
            } else {
                padding = start_str.size();
            }
            std::ostringstream oss_help;
            oss_help << arg->help;
            if(!arg->required) {
                oss_help << " (default: `" << arg->default_string << "`)";
            }
            std::string help = oss_help.str();

            for(int i = 0; i < help.size(); i += 55) {
                std::cerr << std::string(24 - padding, ' ');
                std::cerr << std::string(help.begin() + i, help.begin() + std::min(i + 55, (int)help.size()));
                std::cerr << '\n';
                padding = 0;
            }
        }
    };

    if(!mandatory.empty()) {
        std::cerr << "Mandatory arguments:\n";
        print_arg(mandatory);
        std::cerr << '\n';
    }
    if(!optional.empty()) {
        std::cerr << "Optional arguments:\n";
        print_arg(optional);
    }
}
