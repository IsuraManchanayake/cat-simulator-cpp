//
// Created by isura on 10/12/2021.
//

#include "Simulation.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numbers>
#include <sstream>
#include <vector>

#include "Config.h"
#include "Exceptions.h"
#include "Utils.h"

Simulation::Simulation(InputParams input_params)
        : seed(0), n_steps(0), population(0), hour_of_day(0), neighborhood(Neighborhood::Moore),
          neighborhood_radius(0), continuous_food(false), step(0), width(0), height(0),
          terrain(nullptr), elevations(), cell_types(), current_population(0), results_file_path(),
          log_forces(false), start_time(), input_params(std::move(input_params)) {}

void Simulation::setup() {
    Logger::log() << "Setting up simulation";
    this->start_time = std::chrono::steady_clock::now();
    if (input_params.state_file.empty()) {
        this->_setup_from_args();
    } else {
        this->_setup_from_file();
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - this->start_time).count();

    Logger::log() << "Elapsed " << static_cast<double>(elapsed) / 1000.0 << " s";
}

void Simulation::_setup_from_file() {

}

void Simulation::_setup_from_args() {
    this->seed = input_params.seed;
    this->n_steps = input_params.n_steps;
    this->population = input_params.population;
    this->hour_of_day = input_params.hour_of_day;
    this->neighborhood = input_params.neighborhood;
    this->neighborhood_radius = input_params.neighborhood_radius;
    this->continuous_food = input_params.continuous_food;
    this->results_file_path = input_params.results_file_path;

    this->current_population = this->population;
    this->step = 0;

    bool dimensions_set = false;
    if (!this->input_params.t_elevations_file.empty()) {
        std::ifstream ifs_ele(this->input_params.t_elevations_file);
        this->elevations.clear();
        size_t _width = 0;
        std::string line;
        while (std::getline(ifs_ele, line)) {
            std::vector<double> row;
            std::istringstream iss_row(line);
            double elevation = 0;
            while (iss_row >> elevation) {
                row.push_back(elevation);
            }
            if (_width == 0) {
                _width = row.size();
                if (_width == 0) {
                    throw InvalidSimulationParamsException("Invalid elevation map.");
                }
            } else if (_width != row.size()) {
                throw InvalidSimulationParamsException("Invalid elevation map.");
            }
            this->elevations.push_back(row);
        }
        ifs_ele.close();
        if (this->elevations.empty()) {
            throw InvalidSimulationParamsException("Invalid elevation map.");
        }
        this->height = this->elevations.size();
        this->width = _width;
        dimensions_set = true;
    }
    if (!this->input_params.t_cell_types_file.empty()) {
        std::ifstream ifs_cell(this->input_params.t_cell_types_file);
        this->cell_types.clear();
        size_t _width = 0;
        std::string line;
        while (std::getline(ifs_cell, line)) {
            std::vector<CellType> row;
            std::istringstream iss_row(line);
            char cell_type_char = 0;
            while (iss_row >> cell_type_char) {
                row.push_back(char_to_cell_type(cell_type_char));
            }
            if (_width == 0) {
                _width = row.size();
                if (_width == 0) {
                    throw InvalidSimulationParamsException("Invalid cell type map.");
                }
            } else if (_width != row.size()) {
                throw InvalidSimulationParamsException("Invalid cell type map.");
            }
            this->cell_types.push_back(row);
        }
        if (this->cell_types.empty()) {
            throw InvalidSimulationParamsException("Invalid cell type map.");
        }
        ifs_cell.close();

        if (dimensions_set) {
            if (this->height != this->cell_types.size() || this->width != _width) {
                throw InvalidSimulationParamsException("Elevation map and cell type map dimensions mismatch.");
            }
        } else {
            this->height = this->cell_types.size();
            this->width = _width;
            dimensions_set = true;
        }
    }

    if (!dimensions_set) {
        this->height = this->input_params.t_height;
        this->width = this->input_params.t_width;
    }

    if (this->elevations.empty()) {
        for (size_t i = 0; i < this->height; ++i) {
            this->elevations.emplace_back(0.0, this->width);
        }
    }
    if (this->cell_types.empty()) {
        for (size_t i = 0; i < this->height; ++i) {
            std::vector<CellType> row;
            static const std::map<CellType, double> weights
                    {
                            {CellType::floor, 92.0},
                            {CellType::bed,   3.0},
                            {CellType::box,   2.0},
                            {CellType::food,  4.0}
                    };
            for (size_t j = 0; j < this->width; ++j) {
                row.push_back(Enum<CellType>::random(weights));
            }
        }
    }

    this->terrain = new Terrain(this->width, this->height, this->elevations, this->cell_types);
    for (size_t i = 0; i < this->population; ++i) {
        const auto x = Random::next<size_t>(0, this->terrain->width - 1);
        const auto y = Random::next<size_t>(0, this->terrain->height - 1);
        const auto personality = Enum<Personality>::random();
        const auto gender = Enum<Gender>::random();
        const auto age = static_cast<double>(Random::next<size_t>(0, 10));
        const auto health = max_health(age);
        auto cat = Cat::create(
                Vec2{x, y},
                age,
                gender,
                personality,
                health,
                State::active
        );
        this->terrain->put_cat(cat);
    }
}

bool Simulation::is_finished() const {
    return this->step >= this->n_steps || this->population <= 0;
}

void Simulation::update() {
    auto step_ = this->step + 1;
    Logger::sep();
    Logger::log() << "Starting step: " << step_;
    Logger::log() << "Day: " << step / 24 << " Hour: " << step % 24;
    Logger::log() << "Hour of day; " << this->hour_of_day + 1;
    auto start = std::chrono::steady_clock::now();

    std::cout << "Step " << step << '\n';

    auto next_terrain = new Terrain(this->width, this->height, this->elevations, this->cell_types, this->terrain);
    std::vector<Cat *> next_cats;

    for (size_t y = 0; y < this->height; ++y) {
        for (size_t x = 0; x < this->width; ++x) {
            auto cell = this->terrain->at(x, y);
            if (cell.cell_type == CellType::food) {
                if (this->continuous_food) {
                    cell.food_amount = Config::continuous_food_amount;
                } else if (this->hour_of_day == 12) {
                    cell.food_amount = std::min(Config::start_food_amount, cell.food_amount + Config::new_food_amount);
                }
            }
        }
    }

    for (size_t y = 0; y < this->height; ++y) {
        for (size_t x = 0; x < this->width; ++x) {
            for (auto cat: this->terrain->cats_at(x, y)) {
                this->_pre_tick(cat, next_cats);
            }
        }
    }
    for (size_t y = 0; y < this->height; ++y) {
        for (size_t x = 0; x < this->width; ++x) {
            for (auto cat: this->terrain->cats_at(x, y)) {
                this->_tick(cat);
            }
        }
    }
    for (size_t y = 0; y < this->height; ++y) {
        for (size_t x = 0; x < this->width; ++x) {
            for (auto cat: this->terrain->cats_at(x, y)) {
                this->_post_tick(cat, next_cats);
            }
        }
    }

    for (auto cat: Cat::all) {
        cat->update_state_hours();
    }

    for (auto next_cat: next_cats) {
        next_terrain->put_cat(next_cat);
    }
    this->current_population = next_cats.size();

    delete this->terrain;
    this->terrain = next_terrain;

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    Logger::log() << "Current population: " << this->current_population;
    Logger::log() << "Finished step: " << this->step;
    Logger::log() << "Elapsed " << static_cast<double>(elapsed) / 1000.0 << " s";

    ++this->step;
    this->hour_of_day = (this->hour_of_day + 1) % 24;

    this->save_state();
}

void Simulation::_pre_tick(Cat *cat, std::vector<Cat *> &next_cats) const {
    auto cell = this->terrain->cell_at(cat->position);

    cat->start_step();

    // Sleep if necessary
    if (!cat->is_sleeping()) {
        auto sleep_probability = get_sleep_probability(cell.cell_type, cat->health);
        if (auto random = Random::next<double>(0, 1); random < sleep_probability) {
            cat->sleep();
        }
    }

    // Force wake-up if health is low
    if (cat->is_sleeping() && cat->health < Config::force_wake_up_health) {
        Logger::log() << "[Alert] " << cat << " is forced to wake up";
        cat->wake_up();
    }

    // Wake up if sleep duration is exceeded
    if (cat->is_sleeping() && cat->sleep_duration >= Config::sleep_time) {
        cat->wake_up();
    }

    // Consume food
    if (cell.cell_type == CellType::food && !cat->is_sleeping()) {
        auto consume_amount = std::min(cell.food_amount, std::min(10.0, cat->max_health() - cat->health));
        if (consume_amount > 0) {
            cell.get_consumed(consume_amount);
            cat->consume_food(consume_amount);
        }
    }

    // Deliver off-spring
    if(cat->is_pregnant() && cat->hours_since_last_conception >= Config::hours_to_deliver_offspring) {
        auto cat_baby = cat->deliver();
        next_cats.push_back(cat_baby);
    }
}

void Simulation::_tick(Cat *cat) const {
    auto cell = this->terrain->cell_at(cat->position);

    // Interact
    for (auto other_cat: cell.cats) {
        if (cat->cat_id == other_cat->cat_id) {
            continue;
        }
        cat->interact(other_cat, this->_temperature());
    }

    // -- Calculate forces
    auto force = Vec2::zero();

    // Food force
    auto food_radius = this->neighborhood_radius;
    if (cat->health < 10) {
        food_radius = 3 * this->neighborhood_radius;
    } else if (cat->health < 25) {
        food_radius = 1.5 * this->neighborhood_radius;
    }
    auto neighbors = this->terrain->neighbors(cat->position, food_radius, this->neighborhood);
    for (auto other_cell: neighbors) {
        if (other_cell->cell_type == CellType::food) {
            auto food_attraction = (other_cell->food_amount / 100.0) * cat->food_attraction();
            auto food_force = calc_force(food_attraction, cat->position, other_cell->position);
            force += food_force;
            if (this->log_forces) {
                Logger::log() << "[Force][Food]" << " A force " << food_force << " is exerted on " << cat;
            }
        }
    }

    neighbors = this->terrain->neighbors(cat->position, this->neighborhood_radius, this->neighborhood);
    for (auto other_cell: neighbors) {
        // Bed attraction
        if (other_cell->cell_type == CellType::bed) {
            auto bed_attraction = cat->bed_attraction();
            auto bed_force = calc_force(bed_attraction, cat->position, other_cell->position);
            force += bed_force;
            if (this->log_forces) {
                Logger::log() << "[Force][Bed]" << " A force " << bed_force << " is exerted on " << cat;
            }
        }
        // Box attraction
        if (other_cell->cell_type == CellType::box) {
            auto box_attraction = cat->box_attraction();
            auto box_force = calc_force(box_attraction, cat->position, other_cell->position);
            force += box_force;
            if (this->log_forces) {
                Logger::log() << "[Force][Box]" << " A force " << box_force << " is exerted on " << cat;
            }
        }
        // Mutual attraction
        for (auto other_cat: other_cell->cats) {
            if (other_cat->cat_id == cat->cat_id) {
                continue;
            }
            auto mutual_attraction = cat->mutual_attraction(other_cat, this->_temperature());
            auto mutual_force = calc_force(mutual_attraction, cat->position, other_cell->position);
            force += mutual_force;
            if (this->log_forces) {
                Logger::log() << "[Force][Mutual]" << " A force " << mutual_force << " is exerted on " << cat << " by "
                              << other_cat;
            }
        }
        // Trace attraction
        if (other_cell->x_trace > 0 || other_cell->y_trace > 0) {
            auto trace_attraction = cat->trace_attraction(other_cell->x_trace, other_cell->y_trace);
            auto trace_force = calc_force(trace_attraction, cat->position, other_cell->position);
            force += trace_force;
            if (this->log_forces) {
                Logger::log() << "[Force][Trace]" << " A force " << trace_force << " is exerted on " << cat
                              << " by cell at " << other_cell->position << " x_trace=" << other_cell->x_trace
                              << " y_trace=" << other_cell->y_trace;
            }
        }
    }
    // Randomness
    auto random_force = cat->random_force();
    force += random_force;
    if (this->log_forces) {
        Logger::log() << "[Force][Random]" << " A force " << random_force << " is exerted on " << cat;
    }

    cat->add_force(force);
    if (this->log_forces) {
        Logger::log() << "[Force]" << " A total force of  " << cat->get_force() << " is exerted on " << cat;
    }
}

void Simulation::_post_tick(Cat *cat, std::vector<Cat *> &next_cats) const {
    auto target_position = cat->position + cat->get_force();
    Logger::log() << cat << " trying to move from position " << cat->position << " to target position "
        << cat->position + cat->get_force();
    auto clamped_target_position = this->terrain->clamp(cat->position, target_position);
    Logger::log() << cat << " clamped target position " << clamped_target_position;
    if(cat->position != target_position) {
        auto health_damage = this->terrain->health_damage_to_travel(cat->position, clamped_target_position);
        cat->move(clamped_target_position, health_damage);
    }
    cat->finalize_step();

    if(!cat->should_die()) {
        next_cats.push_back(cat);
    } else {
        cat->die();
    }
}

double Simulation::_temperature() const {
    return 25 - 5 * std::cos(std::numbers::pi * static_cast<double>(this->hour_of_day) / 12.0);
}

void Simulation::save_state() const {

}

void Simulation::finalize() const {
    Logger::sep();
    Logger::log() << "Simulation is finishing";

    size_t alive = 0;
    for (auto cat: Cat::all) {
        alive += cat->is_alive();
    }
    Logger::log() << alive << " cats alive";

    for (auto cat: Cat::all) {
        Logger::log() << cat;
        Logger::log() << cat->summary;
    }

    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - this->start_time).count();
    Logger::log() << "Simulation is finished";
    Logger::log() << "Elapsed " << static_cast<double>(elapsed) / 1000.0 << " s";
}
