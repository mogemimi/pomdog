cmake_minimum_required(VERSION 3.10)
project(Pomdog CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(POMDOG_BUILD_TARGET_ALL ON)
add_subdirectory(build/pomdog)
add_subdirectory(build/pomdog_experimental)
add_subdirectory(examples/FeatureShowcase)
add_subdirectory(examples/Pong)
add_subdirectory(examples/QuickStart)
add_subdirectory(test)
