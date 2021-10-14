//
// Created by isura on 10/13/2021.
//

#ifndef CAT_SIMULATOR_CPP_UTILS_H
#define CAT_SIMULATOR_CPP_UTILS_H

#include "Cat.h"
#include "Enums.h"

#include <map>
#include <ostream>
#include <utility>

CellType char_to_cell_type(char c);
double max_health(double age);
std::pair<Cat *, Cat *> choose_female_cat(Cat *cat1, Cat *cat2);
double get_sleep_probability(CellType cell_type, double health);
Vec2 calc_force(double magnitude, const Vec2 &from_v, const Vec2 &to_v);

template<typename K, typename V>
std::ostream &operator<<(std::ostream &os, const std::map<K, V> &map) {
    os << "{";
    bool first = true;
    for(const auto &pair: map) {
        const auto &key = pair.first;
        const auto &value = pair.second;
        if(!first) {
            os << ", ";
        }
        os << key << ": " << value;
        first = false;
    }
    return os << "}";
}

#endif //CAT_SIMULATOR_CPP_UTILS_H
