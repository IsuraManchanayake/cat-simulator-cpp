//
// Created by isura on 10/12/2021.
//

#ifndef CAT_SIMULATOR_CPP_CONFIG_H
#define CAT_SIMULATOR_CPP_CONFIG_H

struct Config {
    static constexpr double max_trace = 1.0;
    static constexpr double start_food_amount = 100.0;
    static constexpr double continuous_food_amount = 100.0;
    static constexpr double new_food_amount = 30.0;
    static constexpr double trace_attraction_factor = 0.5;
    static constexpr double max_life_span = 10;
    static constexpr double force_wake_up_health = 10;
    static constexpr size_t sleep_time = 4;
    static constexpr size_t hours_to_deliver_offspring = 4;
};

#endif //CAT_SIMULATOR_CPP_CONFIG_H
