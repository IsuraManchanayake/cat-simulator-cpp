//
// Created by isura on 10/12/2021.
//


#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "catsim/Enums.h"


using testing::Eq;
using testing::Contains;

TEST(EnumTestSuite, Count) {
    const auto gender_count = Enum<Gender>::length;
    const auto personality_count = Enum<Personality>::length;
    const auto state_count = Enum<State>::length;

    EXPECT_EQ(gender_count, 2);
    EXPECT_EQ(personality_count, 2);
    EXPECT_EQ(state_count, 4);
}

TEST(EnumTestSuite, All) {
    const auto genders = Enum<Gender>::all;
    const auto states = Enum<State>::all;

    EXPECT_EQ(genders.size(), 2);
    EXPECT_EQ(genders[0], Gender::male);
    EXPECT_EQ(genders[1], Gender::female);
    EXPECT_EQ(states.size(), 4);
    EXPECT_EQ(states[0], State::fetus);
    EXPECT_EQ(states[1], State::active);
    EXPECT_EQ(states[2], State::sleeping);
    EXPECT_EQ(states[3], State::dead);
}

TEST(EnumTestSuite, Previous) {
    const auto prev_dead = Enum<State>::previous<State::dead>;

    EXPECT_EQ(prev_dead, State::sleeping);
}

TEST(EnumTestSuite, After) {
    const auto after_X = Enum<Personality>::after<Personality::X>;

    EXPECT_EQ(after_X, Personality::Y);
}

TEST(EnumTestSuite, Random) {
    const auto rand_state = Enum<State>::random();

    EXPECT_THAT(Enum<State>::all, Contains(rand_state));
}