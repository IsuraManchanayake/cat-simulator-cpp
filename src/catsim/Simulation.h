//
// Created by isura on 10/12/2021.
//

#ifndef CAT_SIMULATOR_CPP_SIMULATION_H
#define CAT_SIMULATOR_CPP_SIMULATION_H

#include "ArgParser.h"
#include "Enums.h"
#include "InputParams.h"
#include "Terrain.h"

#include <chrono>
#include <string>

struct Simulation {
    size_t seed;
    size_t n_steps;
    size_t population;
    size_t hour_of_day;
    Neighborhood neighborhood;
    double neighborhood_radius;
    bool continuous_food;
    size_t step;
    size_t width;
    size_t height;
    Terrain *terrain;
    std::vector<std::vector<double>> elevations;
    std::vector<std::vector<CellType>> cell_types;
    size_t current_population;
    std::string results_file_path;

    bool log_forces;
    std::chrono::steady_clock::time_point start_time;

    const InputParams input_params;

    explicit Simulation(InputParams input_params);

    void setup();
    [[nodiscard]] bool is_finished() const;
    void update();
    void finalize() const;
    void save_state() const;

    void _setup_from_file();
    void _setup_from_args();
    void _pre_tick(Cat *cat, std::vector<Cat *> &next_cats) const;
    void _tick(Cat *cat) const;
    void _post_tick(Cat *cat, std::vector<Cat *> &next_cats) const;
    [[nodiscard]] double _temperature() const;
};


#endif //CAT_SIMULATOR_CPP_SIMULATION_H
