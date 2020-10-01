cmake_minimum_required(VERSION 3.16)
project(Pomdog)

set(POMDOG_BUILD_TARGET_ALL ON)
add_subdirectory(build/pomdog)
add_subdirectory(examples/FeatureShowcase)
add_subdirectory(examples/Pong)
add_subdirectory(examples/QuickStart)
add_subdirectory(test)
