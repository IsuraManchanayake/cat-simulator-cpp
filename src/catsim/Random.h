//
// Created by isura on 10/13/2021.
//

#ifndef CAT_SIMULATOR_CPP_RANDOM_H
#define CAT_SIMULATOR_CPP_RANDOM_H

#include <random>
#include <type_traits>

class Random {
public:
    using seed_type = std::mt19937::result_type;
    template<typename T>
    using dist_type = typename std::conditional<
            std::is_floating_point_v<T>,
            std::uniform_real_distribution<T>,
            std::uniform_int_distribution<T>>::type;

    static void set_seed(seed_type seed);
    template<typename T>
    static T next(T a, T b);

    static std::mt19937 eng;

private:
    static seed_type _seed;
};

template<typename T>
T Random::next(T a, T b) {
    return Random::dist_type<T>(a, b)(Random::eng);
}

#endif //CAT_SIMULATOR_CPP_RANDOM_H
