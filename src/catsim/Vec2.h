//
// Created by isura on 10/12/2021.
//

#ifndef CAT_SIMULATOR_CPP_VEC2_H
#define CAT_SIMULATOR_CPP_VEC2_H

#include <ostream>

struct Vec2 {
    double x;
    double y;

    Vec2();
    template<typename T>
    Vec2(T x, T y);

    [[nodiscard]] Vec2 operator+(const Vec2 &rhs) const;
    [[nodiscard]] Vec2 operator-(const Vec2 &rhs) const;
    [[nodiscard]] Vec2 operator-() const;
    [[nodiscard]] bool operator==(const Vec2 &rhs) const;
    [[nodiscard]] bool operator!=(const Vec2 &rhs) const;
    template<typename T>
    [[nodiscard]] Vec2 operator*(const T &rhs) const;
    Vec2 &operator+=(const Vec2 &rhs);
    Vec2 &operator-=(const Vec2 &rhs);

    [[nodiscard]] double norm() const;
    [[nodiscard]] Vec2 unit() const;
    [[nodiscard]] double dot(const Vec2 &rhs) const;
    [[nodiscard]] double cross(const Vec2 &rhs) const;

    [[nodiscard]] static Vec2 zero();
    [[nodiscard]] static Vec2 one();
};

template<typename T>
Vec2::Vec2(T x, T y) : x(x), y(y) {}

template<typename T>
Vec2 Vec2::operator*(const T &rhs) const {
    return {this->x * rhs, this->y * rhs};
}

template<typename T>
Vec2 operator*(const T &lhs, const Vec2 &rhs) {
    return {lhs * rhs.x, lhs * rhs.y};
}

std::ostream &operator<<(std::ostream &os, const Vec2 &vec2);

#endif //CAT_SIMULATOR_CPP_VEC2_H
