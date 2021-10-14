//
// Created by isura on 10/13/2021.
//

#include "Enums.h"


std::ostream &operator<<(std::ostream &os, Gender gender) {
    switch(gender) {
        case Gender::male:
            os << "male";
            break;
        case Gender::female:
            os << "female";
            break;
        default:
            break;
    }
    return os;
}


std::ostream &operator<<(std::ostream &os, Personality personality) {
    switch(personality) {
        case Personality::X:
            os << "X";
            break;
        case Personality::Y:
            os << "Y";
            break;
        default:
            break;
    }
    return os;
}


std::ostream &operator<<(std::ostream &os, State state) {
    switch(state) {
        case State::fetus:
            os << "fetus";
            break;
        case State::active:
            os << "active";
            break;
        case State::sleeping:
            os << "sleeping";
            break;
        case State::dead:
            os << "dead";
            break;
        default:
            break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, CellType cell_type) {
    switch(cell_type) {
        case CellType::floor:
            os << "floor";
            break;
        case CellType::food:
            os << "food";
            break;
        case CellType::bed:
            os << "bed";
            break;
        case CellType::box:
            os << "box";
            break;
        default:
            break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, Neighborhood neighborhood) {
    switch(neighborhood) {
        case Neighborhood::Moore:
            os << "Moore";
            break;
        case Neighborhood::VonNeumann:
            os << "VonNeumann";
            break;
        default:
            break;
    }
    return os;
}
