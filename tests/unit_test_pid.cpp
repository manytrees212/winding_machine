#include "gtest/gtest.h"
#include "pid.hpp"

#include <cstdio>

TEST(PidTest, CalcSpeedWithDefaultCtor) {
    PID pid;
    EXPECT_EQ(pid.ComputeSpeed(0,0,0), 0);
}

TEST(PidTest, Check_P) {
    PID pid{80, 0, 0, 0, 100};

    pid.ComputeSpeed(50, 0, 50);

    auto pid_monitor_data = pid.GetMonitorData();
    std::cout<<"P="<<pid_monitor_data.p<<'\n';
    std::cout<<"I="<<pid_monitor_data.i<<'\n';
    std::cout<<"D="<<pid_monitor_data.d<<'\n';
    std::cout<<"PID="<<pid_monitor_data.out<<'\n';

    EXPECT_GT(pid_monitor_data.p, 0);
    EXPECT_EQ(pid_monitor_data.i, 0);
    EXPECT_EQ(pid_monitor_data.d, 0);
}

TEST(PidTest, Check_I) {
    PID pid{0, 50, 0, 0, 100};

    pid.ComputeSpeed(100, 0, 50);
    pid.ComputeSpeed(200, 60, 50);

    auto pid_monitor_data = pid.GetMonitorData();
    std::cout<<"P="<<pid_monitor_data.p<<'\n';
    std::cout<<"I="<<pid_monitor_data.i<<'\n';
    std::cout<<"D="<<pid_monitor_data.d<<'\n';
    std::cout<<"PID="<<pid_monitor_data.out<<'\n';

    EXPECT_EQ(pid_monitor_data.p, 0);
    EXPECT_GT(pid_monitor_data.i, 0);
    EXPECT_EQ(pid_monitor_data.d, 0);
}

TEST(PidTest, Check_D) {
    PID pid{0, 0, 20, 0, 100};

    pid.ComputeSpeed(100, 0, 50);
    pid.ComputeSpeed(200, 150, 50);

    auto pid_monitor_data = pid.GetMonitorData();
    std::cout<<"P="<<pid_monitor_data.p<<'\n';
    std::cout<<"I="<<pid_monitor_data.i<<'\n';
    std::cout<<"D="<<pid_monitor_data.d<<'\n';
    std::cout<<"PID="<<pid_monitor_data.out<<'\n';

    EXPECT_EQ(pid_monitor_data.p, 0);
    EXPECT_EQ(pid_monitor_data.i, 0);
    EXPECT_LT(pid_monitor_data.d, 0);
}

TEST(PidTest, Check_Behavior) {
    PID pid{80, 5, 20, 0, 100};

    auto speed = pid.ComputeSpeed(100, 0, 50);
    std::cout<<"speed="<<speed<<'\n';
    EXPECT_GT(speed, 0);

    pid.ComputeSpeed(120, 20, 50);
    pid.ComputeSpeed(140, 30, 50);
    pid.ComputeSpeed(150, 40, 50);

    speed=pid.ComputeSpeed(155, 50, 50);
    std::cout<<"speed="<<speed<<'\n';
    EXPECT_LT((pid.GetMonitorData()).d, 0);

    speed = pid.ComputeSpeed(180, 80, 50);
    std::cout<<"speed="<<speed<<'\n';
    EXPECT_NEAR(speed, 20, 10);
}

TEST(PidTest, Check_Max_I){
    PID pid{0, 100, 0, 0, 100};

    pid.ComputeSpeed(100, 0, 50);
    pid.ComputeSpeed(200, 5, 50);
    std::cout<<"I="<<(pid.GetMonitorData()).i<<'\n';
    pid.ComputeSpeed(300, 6, 50);
    std::cout<<"I="<<(pid.GetMonitorData()).i<<'\n';
    pid.ComputeSpeed(400, 7, 50);
    std::cout<<"I="<<(pid.GetMonitorData()).i<<'\n';
    pid.ComputeSpeed(500, 8, 50);
    std::cout<<"I="<<(pid.GetMonitorData()).i<<'\n';
    pid.ComputeSpeed(600, 9, 50);
    std::cout<<"I="<<(pid.GetMonitorData()).i<<'\n';
    pid.ComputeSpeed(700, 10, 50);
    std::cout<<"I="<<(pid.GetMonitorData()).i<<'\n';
    pid.ComputeSpeed(800, 10, 50);
    std::cout<<"I="<<(pid.GetMonitorData()).i<<'\n';

    EXPECT_EQ((pid.GetMonitorData()).i, 80);
}

TEST(PidTest, CheckResetMethod){

    PID pid{80, 50, 50, 0, 100};

    pid.ComputeSpeed(100, 0, 50);
    pid.ComputeSpeed(200, 5, 50);
    pid.ComputeSpeed(300, 6, 50);
    pid.ComputeSpeed(400, 7, 50);

    std::cout<<"I="<<(pid.GetMonitorData()).i<<'\n';
    std::cout<<"D="<<(pid.GetMonitorData()).d<<'\n';

    EXPECT_GT(pid.GetIntegral(),0);
    EXPECT_LT(pid.GetDerivFiltered(),0);
    EXPECT_GT(pid.GetLastOutput(),0);
    EXPECT_GT(pid.GetPrevInput(),0);

    pid.SetGains(80, 50, 50);
    EXPECT_EQ(pid.GetIntegral(),0);
    EXPECT_EQ(pid.GetDerivFiltered(),0);
    EXPECT_EQ(pid.GetLastOutput(),0);
    EXPECT_EQ(pid.GetPrevInput(),0);
}


