//
// Created by isura on 10/13/2021.
//

#include "Utils.h"

CellType char_to_cell_type(char c) {
    switch (c) {
        case '.':
            return CellType::floor;
        case 'F':
            return CellType::food;
        case 'B':
            return CellType::bed;
        case 'b':
            return CellType::box;
        default:
            break;
    }
    return CellType::floor;
}

double max_health(double age) {
    if (age < 1) {
        return 50;
    }
    return 100;
}

std::pair<Cat *, Cat *> choose_female_cat(Cat *cat1, Cat *cat2) {
    return cat1->gender == Gender::female ? std::make_pair(cat1, cat2) : std::make_pair(cat2, cat1);
}

double get_sleep_probability(CellType cell_type, double health) {
    double p = 0;
    switch (cell_type) {
        case CellType::floor:
            p = 0.05;
            break;
        case CellType::food:
            p = 0.01;
            break;
        case CellType::bed:
            p = 0.2;
            break;
        case CellType::box:
            p = 0.4;
            break;
        default:
            break;
    }
    if(health > 95){
        p *= 1.02;
    }
    p = std::min(1.0, p);
    return p;
}

Vec2 calc_force(double magnitude, const Vec2 &from_v, const Vec2 &to_v) {
    return magnitude * (to_v - from_v).unit();
}
