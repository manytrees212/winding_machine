#include "gtest/gtest.h"
#include "pid.hpp"

TEST(PidTest, SimpleTest) {
    PID pid;
    EXPECT_EQ(pid.ComputeSpeed(0,0,0), 0);
}
