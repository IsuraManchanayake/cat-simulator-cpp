//
// Created by isura on 10/12/2021.
//

#include <cmath>

#include "Vec2.h"

Vec2::Vec2() : x(0), y(0) {}

Vec2 Vec2::operator+(const Vec2 &rhs) const {
    return {this->x + rhs.x, this->y + rhs.y};
}

Vec2 Vec2::operator-(const Vec2 &rhs) const {
    return {this->x - rhs.x, this->y - rhs.y};
}

Vec2 Vec2::operator-() const {
    return {-this->x, -this->y};
}

bool Vec2::operator==(const Vec2 &rhs) const {
    return this->x == rhs.x && this->y == rhs.y;
}

bool Vec2::operator!=(const Vec2 &rhs) const {
    return this->x != rhs.x || this->y != rhs.y;
}

Vec2 &Vec2::operator+=(const Vec2 &rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

Vec2 &Vec2::operator-=(const Vec2 &rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

double Vec2::norm() const {
    return hypot(this->x, this->y);
}

Vec2 Vec2::unit() const {
    const auto norm = this->norm();
    if(norm == 0) {
        return {0, 0};
    }
    return (1 / norm) * (*this);
}

double Vec2::dot(const Vec2 &rhs) const {
    return this->x * rhs.x + this->y * rhs.y;
}

double Vec2::cross(const Vec2 &rhs) const {
    return this->x * rhs.y - this->y * rhs.x;
}

Vec2 Vec2::zero() { return {}; }
Vec2 Vec2::one() { return {1, 1}; }

std::ostream &operator<<(std::ostream &os, const Vec2 &vec2) {
    return os << "{x=" << vec2.x << ",y=" << vec2.y << "}";
}
