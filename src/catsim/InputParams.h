//
// Created by isura on 10/13/2021.
//

#ifndef CAT_SIMULATOR_CPP_INPUTPARAMS_H
#define CAT_SIMULATOR_CPP_INPUTPARAMS_H

#include <string>

#include "Enums.h"

struct InputParams {
    std::string state_file;
    size_t population;
    size_t n_steps;
    size_t t_width;
    size_t t_height;
    size_t hour_of_day;
    Neighborhood neighborhood;
    double neighborhood_radius;
    bool continuous_food;
    std::string t_elevations_file;
    std::string t_cell_types_file;
    std::string log_file_path;
    std::string results_file_path;
    int seed;

    InputParams(std::string state_file, size_t population, size_t n_steps, size_t t_width, size_t t_height,
                size_t hour_of_day, Neighborhood neighborhood, double neighborhood_radius, bool continuous_food,
                std::string t_elevations_file, std::string t_cell_types_file, std::string log_file_path,
                std::string results_file_path, int seed);

    static InputParams from_args(int argc, const char **argv);
};


#endif //CAT_SIMULATOR_CPP_INPUTPARAMS_H
