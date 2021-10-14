//
// Created by isura on 10/12/2021.
//

#include "Terrain.h"

#include "Config.h"

Cell::Cell(const Vec2 &position, const std::vector<Cat *> &cats, double x_trace, double y_trace, CellType cell_type,
           double food_amount, double elevation)
        : position(position), cats(cats), x_trace(x_trace), y_trace(y_trace), cell_type(cell_type),
          food_amount(food_amount), elevation(elevation) {
}

void Cell::increment_trace(Personality personality, double value) {
    if (personality == Personality::X) {
        x_trace = std::min(Config::max_trace, this->x_trace + value);
    } else {
        y_trace = std::min(Config::max_trace, this->y_trace + value);
    }
}

void Cell::put_cat(Cat *const cat) {
    this->cats.push_back(cat);
    this->increment_trace(cat->personality, 1);
}

void Cell::get_consumed(double consumed) {
    this->food_amount = std::max(0.0, this->food_amount - consumed);
}

Terrain::Terrain(size_t width, size_t height, const std::vector<std::vector<double>> &elevations,
                 const std::vector<std::vector<CellType>> &cell_types, Terrain *previous_terrain)
        : width(width), height(height), grid() {
    for (size_t y = 0; y < height; ++y) {
        std::vector<Cell> row;
        for (size_t x = 0; x < width; ++x) {
            const auto &cell_type = cell_types[y][x];
            double food_amount = cell_type == CellType::food ? Config::start_food_amount : 0.0;
            double x_trace = 0.0;
            double y_trace = 0.0;
            if (previous_terrain) {
                food_amount = previous_terrain->grid[y][x].food_amount;
                x_trace = previous_terrain->grid[y][x].x_trace;
                y_trace = previous_terrain->grid[y][x].y_trace;
            }
//            auto cell = Cell(Vec2(x, y), std::vector<Cat *>(), x_trace, y_trace, cell_type,
//                             food_amount, elevations[y][x]);
//            row.push_back(cell);
            row.emplace_back(Vec2(x, y), std::vector<Cat *>(), x_trace, y_trace, cell_type,
                             food_amount, elevations[y][x]);
        }
        this->grid.push_back(row);
    }
}

Cell &Terrain::at(size_t x, size_t y) {
    return this->grid[y][x];
}

Cell &Terrain::cell_at(const Vec2 &pos) {
    const auto x = static_cast<size_t>(pos.x);
    const auto y = static_cast<size_t>(pos.y);
    return this->at(x, y);
}

const Cell &Terrain::at(size_t x, size_t y) const {
    return this->grid[y][x];
}

const Cell &Terrain::cell_at(const Vec2 &pos) const {
    const auto x = static_cast<size_t>(pos.x);
    const auto y = static_cast<size_t>(pos.y);
    return this->at(x, y);
}

const std::vector<Cat *> &Terrain::cats_at(const Vec2 &pos) const {
    return this->cell_at(pos).cats;
}

const std::vector<Cat *> &Terrain::cats_at(size_t x, size_t y) const {
    return this->at(x, y).cats;
}

bool Terrain::is_position_valid(const Vec2 &vec) const {
    return vec.x >= 0 && vec.x < static_cast<double>(this->width) && vec.y >= 0 &&
           vec.y < static_cast<double>(this->height);
}

bool Terrain::is_position_valid(size_t x, size_t y) const {
    return x >= 0 && x < this->width && y >= 0 && y < this->height;
}

void Terrain::put_cat(Cat *const cat) {
    if (this->is_position_valid(cat->position)) {
        this->cell_at(cat->position).put_cat(cat);
    }
}

std::vector<Cell *> Terrain::neighbors(const Vec2 &center, double radius, Neighborhood neighborhood) {
    int r = static_cast<int>(std::round(radius));
    std::vector<Cell *> cells;
    if (neighborhood == Neighborhood::Moore) {
        cells.reserve((2 * r + 1) * (2 * r + 1));
        for (int dy = -r; dy <= r; ++dy) {
            for (int dx = -r; dx <= r; ++dx) {
                auto position = center + Vec2{dx, dy};
                if (this->is_position_valid(position)) {
                    cells.push_back(&this->cell_at(position));
                }
            }
        }
    } else if (neighborhood == Neighborhood::VonNeumann) {
        cells.reserve(r * r + (r + 1) * (r + 1));
        for (int dy = -r; dy <= r; ++dy) {
            for (int dx = -r; dx <= r; ++dx) {
                if (std::abs(dx) + std::abs(dy) <= r) {
                    auto position = center + Vec2{dx, dy};
                    if (this->is_position_valid(position)) {
                        cells.push_back(&this->cell_at(position));
                    }
                }
            }
        }
    }
    return cells;
}

//#include <iostream>

Vec2 Terrain::make_lattice(const Vec2 &pos) const {
    auto x = std::round(std::max(0.0, std::min(pos.x, static_cast<double>(this->width - 1))));
    auto y = std::round(std::max(0.0, std::min(pos.y, static_cast<double>(this->height - 1))));
//    std::cout << x << ' ' << y << '\n';
    return {x, y};
}

Vec2 Terrain::clamp(const Vec2 &from_v, const Vec2 &to_v) const {
    return this->make_lattice(this->_clamp_destination(from_v, to_v));
}

Vec2 Terrain::_clamp_destination(const Vec2 &from_v, const Vec2 &to_v) const {
    auto h_1 = static_cast<double>(this->height - 1);
    auto w_1 = static_cast<double>(this->width - 1);
    auto p1 = Vec2{0.0, 0.0};
    auto p2 = Vec2{w_1, 0.0};
    auto p3 = Vec2{0.0, h_1};
    auto p4 = Vec2{w_1, h_1};
    std::array<std::array<Vec2, 2>, 4> boundaries{{
          {{p1, p2}},
          {{p1, p3}},
          {{p4, -p2}},
          {{p4, -p3}},
    }};
    auto p = from_v;
    auto r = to_v - from_v;
    for(auto line: boundaries) {
        auto q = line[0];
        auto s = line[1];
        auto q_p = q - p;
        auto q_pxr = q_p.cross(r);
        auto q_pxs = q_p.cross(s);
        auto rxs = r.cross(s);
        if(q_pxs == 0.0) {
            // Starting position is on the boundary
            if(p == q || p == q + s) {
                // Starting point is a corner point
                if(!this->is_position_valid(p + r)) {
                    return p;
                }
                auto c = Vec2{this->width / 2, this->height / 2};
                auto a1 = s.cross(c - q);
                auto a2 = s.cross(p + r - q);
                if(a2 == 0) {
                    // Both p and p + r on the boundary line i.e. co-linear
                    auto u1 = (p + r - q).dot(s) / s.dot(s);
                    if(u1 < 0) {
                        return q;
                    } else if(u1 <= 1) {
                        return p + r;
                    } else {
                        return q + s;
                    }
                }
                if(a1 * a2 < 0) {
                    // Different sides of the boundary line
                    return p;
                } else {
                    continue;
                }
            }
        }
        if(q_pxr == 0.0 && rxs == 0.0) {
            // Co-linear
            auto u1 = (p + r - q).dot(s) / s.dot(s);
            if(u1 < 0) {
                return q;
            } else if(u1 <= 1) {
                return p + r;
            } else {
                return q + s;
            }
        }
        if(rxs == 0.0) {
            continue; // Parallel, No intersection
        }
        auto u = q_pxr / rxs;
        auto t = q_pxs / rxs;
        if(u >= 0 && u <= 1 && t >= 0 && t <= 1) {
            return p + r * t; // If intersects, return
        }
    }
    return p + r; // Intersects with none. ALl good!
}

double Terrain::health_damage_to_travel(const Vec2 &from_v, const Vec2 &to_v) const {
    auto elevation_difference = this->cell_at(to_v).elevation - this->cell_at(from_v).elevation;
    return 0.1 * (std::max(0.0, elevation_difference) + (to_v - from_v).norm());
}
