//
// Created by isura on 10/12/2021.
//

#include "catsim/InputParams.h"
#include "catsim/Simulation.h"
#include "catsim/Logger.h"

int main(int argc, const char** argv) {
    const auto input_params = InputParams::from_args(argc, argv);

    Logger::setup(LogMethod::file);
//    size_t s = 0;
//    for(size_t i = 0; i < 20000; s += i, ++i) {
//        Logger::log() << "Hello " << i;
//    }
//    std::cout << s << '\n';

    auto simulation = Simulation(input_params);
    simulation.setup();
    while(!simulation.is_finished()) {
        simulation.update();
    }
    simulation.finalize();

    return 0;
}
