cmake_minimum_required(VERSION 3.20)

include(CTest)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY git@github.com:google/googletest.git
  GIT_TAG       063de7e9578f82b369302001269680b4b1553359
)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests)

target_sources(unit_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_test_pid.cpp
)

target_link_libraries(unit_tests PRIVATE
  gtest
  gmock
  gtest_main
  pid
)

add_test(NAME "UnitTest_PID" COMMAND $<TARGET_FILE:unit_tests>)
