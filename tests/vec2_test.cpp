//
// Created by isura on 10/12/2021.
//

#include <gtest/gtest.h>

#include "catsim/Vec2.h"


using testing::Eq;

void expect_eq_vec(const Vec2 &v1, const Vec2 &v2) {
    EXPECT_NEAR(v1.x, v2.x, 1e-8);
    EXPECT_NEAR(v1.y, v2.y, 1e-8);
}

TEST(Vec2TestSuite, Add){
    const Vec2 v1{1, 2};
    const Vec2 v2{1.2, 3.1};
    const auto v3 = v1 + v2;

    expect_eq_vec(v3, {2.2, 5.1});
}

TEST(Vec2TestSuite, MultiplyRHS){
    const Vec2 v1{1, 2};
    const auto v2 = v1 * 1.5;

    expect_eq_vec(v2, {1.5, 3.0});
}

TEST(Vec2TestSuite, MultiplyLHS){
    const Vec2 v1{1, 2};
    const auto v2 = 1.5 * v1;

    expect_eq_vec(v2, {1.5, 3.0});
}

TEST(Vec2TestSuite, Substract){
    const Vec2 v1{1, 2};
    const Vec2 v2{1.2, 3.1};
    const auto v3 = v2 - v1;

    expect_eq_vec(v3, {0.2, 1.1});
}

TEST(Vec2TestSuite, Negate){
    const Vec2 v1{1, 2};
    const auto v2 = -v1;

    expect_eq_vec(v2, {-1, -2});
}

TEST(Vec2TestSuite, Equal){
    const Vec2 v1{1, 2};
    const Vec2 v2{1.0, 2.0};
    const auto res = v1 == v2;

    EXPECT_TRUE(res);
}

TEST(Vec2TestSuite, Norm){
    const Vec2 v1{3, 4};
    const auto norm = v1.norm();

    EXPECT_DOUBLE_EQ(norm, 5.0);
}

TEST(Vec2TestSuite, Unit){
    const Vec2 v1{3, 4};
    const auto v2 = v1.unit();

    expect_eq_vec(v2, {0.6, 0.8});
}

TEST(Vec2TestSuite, Dot){
    const Vec2 v1{1, 2};
    const Vec2 v2{3, 4};
    const auto dot1 = v1.dot(v2);
    const auto dot2 = v2.dot(v1);

    EXPECT_DOUBLE_EQ(dot1, 11.0);
    EXPECT_DOUBLE_EQ(dot2, 11.0);
}

TEST(Vec2TestSuite, Cross){
    const Vec2 v1{1, 2};
    const Vec2 v2{2, -1};
    const auto cross1 = v1.cross(v2);
    const auto cross2 = v2.cross(v1);

    EXPECT_DOUBLE_EQ(cross1, -5.0);
    EXPECT_DOUBLE_EQ(cross2, 5.0);
}

TEST(Vec2TestSuite, Zero){
    const Vec2 v1 = Vec2::zero();

    expect_eq_vec(v1, {0.0, 0.0});
}


TEST(Vec2TestSuite, One){
    const Vec2 v1 = Vec2::one();

    expect_eq_vec(v1, {1.0, 1.0});
}
