#include "../../src/vehicle.cpp"
#include <gtest/gtest.h>

TEST(VehicleTest, JustTest) {
    ASSERT_EQ(1, 1);
}



TEST(VehicleTest, KeepLane) {
    Vehicle vehicle;
    auto state = vehicle.get_state();
    ASSERT_EQ(State::LK, state);
}

TEST(VehicleTest, GetNextStates) {
    Vehicle vehicle;
    vector<State> states = vehicle.successor_states();
    ASSERT_EQ(3, states.size());
    ASSERT_EQ(State::LK, states.at(0));
    ASSERT_EQ(State::PLCL, states.at(1));
    ASSERT_EQ(State::PLCR, states.at(2));
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

