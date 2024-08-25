cmake_minimum_required(VERSION 3.27)
project(pomdog)

set(POMDOG_BUILD_TARGET_ALL ON)
add_subdirectory(cmake/pomdog)
add_subdirectory(examples/feature_showcase)
add_subdirectory(examples/pong)
add_subdirectory(examples/quickstart)
add_subdirectory(test)
