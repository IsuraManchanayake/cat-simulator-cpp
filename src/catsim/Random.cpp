//
// Created by isura on 10/13/2021.
//

#include "Random.h"

//Random *Random::_instance = nullptr;
Random::seed_type Random::_seed{};
std::mt19937 Random::eng{Random::_seed};

void Random::set_seed(Random::seed_type seed) {
    Random::_seed = seed;
    eng.seed(seed);
}

