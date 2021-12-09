cmake_minimum_required(VERSION 3.20)
project(pomdog)

set(POMDOG_BUILD_TARGET_ALL ON)
add_subdirectory(cmake/pomdog)
add_subdirectory(examples/FeatureShowcase)
add_subdirectory(examples/Pong)
add_subdirectory(examples/QuickStart)
add_subdirectory(test)
