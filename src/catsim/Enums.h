//
// Created by isura on 10/12/2021.
//

#ifndef CAT_SIMULATOR_CPP_ENUMS_H
#define CAT_SIMULATOR_CPP_ENUMS_H

#include <array>
#include <ostream>
#include <map>

#include "Random.h"

enum class Gender {
    male,
    female,
    _count [[maybe_unused]]
};

enum class Personality {
    X,
    Y,
    _count
};

enum class State {
    fetus,
    active,
    sleeping,
    dead,
    _count
};

enum class CellType {
    floor,
    food,
    bed,
    box,
    _count
};

enum class Neighborhood {
    Moore,
    VonNeumann,
    _count
};

enum class LogMethod {
    none,
    console,
    file,
    _count,
};

namespace EnumHelper {
    template<typename E, E e>
    struct Previous {
        static const E value = static_cast<E>(static_cast<size_t>(e) - 1);
    };

    template<typename E, E e>
    struct After {
        static const E value = static_cast<E>(static_cast<size_t>(e) + 1);
    };

    template<typename E>
    struct Length {
        static const size_t value = static_cast<size_t>(E::_count);
    };

    template<typename E>
    struct Array {
        using type = std::array<E, Length<E>::value>;
//    using type = E[Length<E>::value];
    };

    template<typename E, size_t N, E e, E... args>
    struct EnumArrayHelper {
        static constexpr typename Array<E>::type array = EnumArrayHelper<E,
                N - 1, Previous<E, e>::value, e, args...>::array;
    };

    template<typename E, E e, E... args>
    struct EnumArrayHelper<E, 0, e, args...> {
        static constexpr typename Array<E>::type array = {e, args...};;
    };

    template<typename E>
    struct EnumArray {
        static constexpr typename Array<E>::type array
                = EnumArrayHelper<E, Length<E>::value - 1, Previous<E, E::_count>::value>::array;
    };
}


template<typename E>
struct Enum {
    static const size_t length = EnumHelper::Length<E>::value;
    template<E e>
    static const E previous = EnumHelper::Previous<E, e>::value;
    template<E e>
    static const E after = EnumHelper::After<E, e>::value;

    static constexpr std::array<E, Enum<E>::length> all = EnumHelper::EnumArray<E>::array;

    static E random(std::map<E, double> e_weights = {}) {
        if (e_weights.empty()) {
            const auto r = Random::next<size_t>(0, Enum<E>::length - 1);
            return static_cast<E>(r);
        } else {
            for (const auto &e: Enum<E>::all) {
                if (e_weights.find(e) == e_weights.end()) {
                    e_weights.insert({e, 0});
                }
            }
        }

        const auto r = Random::next<double>(0, 1);
        double w_sum = 0;
        for (const auto &e_weight: e_weights) {
            const auto &e = e_weight.first;
            const auto &w = e_weight.second;
            w_sum += w;
            if (r < w_sum) {
                return e;
            }
        }
        return static_cast<E>(Enum<E>::length - 1);
    }
};

std::ostream &operator<<(std::ostream &os, Gender neighborhood);
std::ostream &operator<<(std::ostream &os, Personality personality);
std::ostream &operator<<(std::ostream &os, State state);
std::ostream &operator<<(std::ostream &os, CellType cell_type);
std::ostream &operator<<(std::ostream &os, Neighborhood neighborhood);

#endif //CAT_SIMULATOR_CPP_ENUMS_H
