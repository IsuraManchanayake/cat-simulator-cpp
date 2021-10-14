//
// Created by isura on 10/12/2021.
//

#ifndef CAT_SIMULATOR_CPP_TERRAIN_H
#define CAT_SIMULATOR_CPP_TERRAIN_H

#include "Cat.h"
#include "Enums.h"
#include "Vec2.h"

struct Cell {
    Vec2 position;
    std::vector<Cat *> cats;
    double x_trace;
    double y_trace;
    const CellType cell_type;
    double food_amount;
    const double elevation;

    Cell(const Vec2 &position, const std::vector<Cat *> &cats, double x_trace = 0, double y_trace = 0,
         CellType cell_type = CellType::floor, double food_amount = 0, double elevation = 0);

    void put_cat(Cat *cat);

    void increment_trace(Personality personality, double value);
    void get_consumed(double consumed_amount);
};

struct Terrain {
    const size_t width;
    const size_t height;
    std::vector<std::vector<Cell>> grid;

    Terrain(size_t width, size_t height, const std::vector<std::vector<double>> &elevations,
            const std::vector<std::vector<CellType>> &cell_types, Terrain *previous_terrain = nullptr);

    Cell &at(size_t x, size_t y);
    Cell &cell_at(const Vec2 &pos);
    const Cell &at(size_t x, size_t y) const;
    const Cell &cell_at(const Vec2 &pos) const;
    const std::vector<Cat *> &cats_at(const Vec2 &pos) const;
    const std::vector<Cat *> &cats_at(size_t x, size_t y) const;
    bool is_position_valid(const Vec2 &vec) const;
    bool is_position_valid(size_t x, size_t y) const;
    Vec2 clamp(const Vec2 &from_v, const Vec2 &to_v) const;
    Vec2 make_lattice(const Vec2 &pos) const;
    Vec2 _clamp_destination(const Vec2 &from_v, const Vec2 &to_v) const;
    double health_damage_to_travel(const Vec2 &from_v, const Vec2 &to_v) const;

    std::vector<Cell *> neighbors(const Vec2 &center, double radius, Neighborhood neighborhood);

    void put_cat(Cat *cat);
};


#endif //CAT_SIMULATOR_CPP_TERRAIN_H
