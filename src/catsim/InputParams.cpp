//
// Created by isura on 10/13/2021.
//

#include "InputParams.h"

#include <cstdlib>

#include "ArgParser.h"
#include "Exceptions.h"


InputParams::InputParams(std::string state_file, size_t population, size_t n_steps, size_t t_width, size_t t_height,
                         size_t hour_of_day, Neighborhood neighborhood, double neighborhood_radius,
                         bool continuous_food, std::string t_elevations_file, std::string t_cell_types_file,
                         std::string log_file_path, std::string results_file_path, int seed)
        : state_file(std::move(state_file)), population(population), n_steps(n_steps), t_width(t_width),
          t_height(t_height), hour_of_day(hour_of_day), neighborhood(neighborhood),
          neighborhood_radius(neighborhood_radius), continuous_food(continuous_food),
          t_elevations_file(std::move(t_elevations_file)), t_cell_types_file(std::move(t_cell_types_file)),
          log_file_path(std::move(log_file_path)), results_file_path(std::move(results_file_path)), seed(seed) {
}

InputParams InputParams::from_args(int argc, const char **argv) {
    ArgParser arg_parser(argc, argv);

    arg_parser.add_parser<std::string>(
            "--state_file",
            "Load simulation from state file.",
            false,
            ""
    );
    arg_parser.add_parser<size_t>(
            "--population",
            "Starting cat population.",
            false,
            10
    );
    arg_parser.add_parser<size_t>(
            "--n_steps",
            "Number of time steps."
    );
    arg_parser.add_parser<size_t>(
            "--t_width",
            "Terrain width.",
            false,
            10
    );
    arg_parser.add_parser<size_t>(
            "--hour_of_day",
            "Starting hour of simulation.",
            false,
            0
    );
    arg_parser.add_parser<Neighborhood>(
            "--neighborhood",
            "Load simulation from state file.",
            false,
            Neighborhood::Moore
    );
    arg_parser.add_parser<double>(
            "--neighborhood_radius",
            "Neighborhood radius.",
            false,
            3.0
    );
    arg_parser.add_parser<bool>(
            "--continuous_food",
            "Flag to set continuous food suppy at food locations. If not set, food will be set periodically at "
            "the 12th hour of the day.",
            false,
            false
    );
    arg_parser.add_parser<std::string>(
            "--t_elevations_file",
            "Terrain elevations as a text file. If specified, t_height and t_width are ignored. If not "
            "specified, elevations are set to 0. If invalid, the simulation exits.",
            false,
            ""
    );
    arg_parser.add_parser<std::string>(
            "--t_cell_types_file",
            "Terrain cell types as a text file. If specified, t_height and t_width are ignored. If not "
            "specified, cell types are chosen random. If invalid, the simulation exits.",
            false,
            ""
    );
    arg_parser.add_parser<std::string>(
            "--log_file_path",
            "All logs go into this file.",
            false,
            "simulation.log"
    );
    arg_parser.add_parser<std::string>(
            "--results_file_path",
            "Simulation results as json file.",
            false,
            "simulation-results.json"
    );
    arg_parser.add_parser<int>(
            "--seed",
            "Random seed.",
            false,
            0
    );
    try {
        arg_parser.parse();
    } catch (const ArgumentException& e) {
        std::cerr << e.what() << "\n\n";
        arg_parser.display_help();
        std::exit(EXIT_FAILURE);
    };

    InputParams params(
            arg_parser.get_value<std::string>("--state_file"),
            arg_parser.get_value<size_t>("--population"),
            arg_parser.get_value<size_t>("--n_steps"),
            arg_parser.get_value<size_t>("--t_width"),
            arg_parser.get_value<size_t>("--t_height"),
            arg_parser.get_value<size_t>("--hour_of_day"),
            arg_parser.get_value<Neighborhood>("--neighborhood"),
            arg_parser.get_value<size_t>("--neighborhood_radius"),
            arg_parser.get_value<bool>("--continuous_food"),
            arg_parser.get_value<std::string>("--t_elevations_file"),
            arg_parser.get_value<std::string>("--t_cell_types_file"),
            arg_parser.get_value<std::string>("--log_file_path"),
            arg_parser.get_value<std::string>("--results_file_path"),
            arg_parser.get_value<int>("--seed")
    );
    return params;
}
