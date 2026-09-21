cmake_minimum_required(VERSION 3.31)
project(pomdog_engine_library_link_tests CXX)

string(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

cmake_path(SET POMDOG_DIR NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
cmake_path(SET TEST_DIR   NORMALIZE "${POMDOG_DIR}/tests")
add_subdirectory(${POMDOG_DIR}/cmake/pomdog pomdog EXCLUDE_FROM_ALL)
enable_testing()

# NOTE: Each consumer links an engine library alias without pomdog_app_settings.
# Reuse behavior tests so each executable must resolve real engine library symbols.
add_executable(math_link_test
    ${TEST_DIR}/testing/main.cpp
    ${TEST_DIR}/math/vector2_test.cpp
)
target_link_libraries(math_link_test PRIVATE pomdog::math)
target_include_directories(math_link_test PRIVATE ${POMDOG_DIR}/thirdparty/doctest)
add_test(NAME math_link COMMAND math_link_test)

add_executable(random_link_test
    ${TEST_DIR}/testing/main.cpp
    ${TEST_DIR}/random/xoroshiro128_star_star_test.cpp
)
target_link_libraries(random_link_test PRIVATE pomdog::random)
target_include_directories(random_link_test PRIVATE ${POMDOG_DIR}/thirdparty/doctest)
add_test(NAME random_link COMMAND random_link_test)

add_executable(vfs_link_test
    ${TEST_DIR}/testing/main.cpp
    ${TEST_DIR}/vfs/file_system_test.cpp
)
target_link_libraries(vfs_link_test PRIVATE pomdog::vfs)
target_include_directories(vfs_link_test PRIVATE ${POMDOG_DIR}/thirdparty/doctest)
add_test(NAME vfs_link COMMAND vfs_link_test)

add_executable(gpu_link_test
    ${TEST_DIR}/testing/main.cpp
    ${TEST_DIR}/gpu/input_layout_builder_test.cpp
)
target_link_libraries(gpu_link_test PRIVATE pomdog::gpu)
target_include_directories(gpu_link_test PRIVATE ${POMDOG_DIR}/thirdparty/doctest)
add_test(NAME gpu_link COMMAND gpu_link_test)

add_executable(graphics_link_test
    ${TEST_DIR}/testing/main.cpp
    ${TEST_DIR}/graphics/sprite_font_sdf_test.cpp
)
target_link_libraries(graphics_link_test PRIVATE pomdog::graphics)
target_include_directories(graphics_link_test PRIVATE ${POMDOG_DIR}/thirdparty/doctest)
add_test(NAME graphics_link COMMAND graphics_link_test)

add_executable(application_link_test
    ${TEST_DIR}/testing/main.cpp
    ${TEST_DIR}/application/frame_rate_limiter_test.cpp
)
target_link_libraries(application_link_test PRIVATE pomdog::application)
target_include_directories(application_link_test PRIVATE ${POMDOG_DIR}/thirdparty/doctest)
add_test(NAME application_link COMMAND application_link_test)

add_executable(experimental_gui_link_test
    ${TEST_DIR}/testing/main.cpp
    ${TEST_DIR}/experimental/gui/drawing_context_test.cpp
)
target_link_libraries(experimental_gui_link_test PRIVATE pomdog::experimental_gui)
target_include_directories(experimental_gui_link_test PRIVATE ${POMDOG_DIR}/thirdparty/doctest)
add_test(NAME experimental_gui_link COMMAND experimental_gui_link_test)

if(NOT EMSCRIPTEN)
    add_executable(experimental_async_link_test
        ${TEST_DIR}/testing/main.cpp
        ${TEST_DIR}/experimental/async/immediate_scheduler_test.cpp
    )
    target_link_libraries(experimental_async_link_test PRIVATE pomdog::experimental_async)
    target_include_directories(experimental_async_link_test PRIVATE ${POMDOG_DIR}/thirdparty/doctest)
    add_test(NAME experimental_async_link COMMAND experimental_async_link_test)
endif()

set_target_properties(
    math_link_test
    random_link_test
    vfs_link_test
    gpu_link_test
    graphics_link_test
    application_link_test
    experimental_gui_link_test

    PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_SCAN_FOR_MODULES OFF
)
if(NOT EMSCRIPTEN)
    set_target_properties(experimental_async_link_test PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_SCAN_FOR_MODULES OFF
    )
endif()

# NOTE: Keep the consumer CRT choice explicit, even though base propagates it.
add_library(engine_library_link_test_settings INTERFACE)
target_compile_options(engine_library_link_test_settings INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:$<IF:$<CONFIG:Debug>,/MTd,/MT>>
)
target_link_libraries(math_link_test PRIVATE engine_library_link_test_settings)
target_link_libraries(random_link_test PRIVATE engine_library_link_test_settings)
target_link_libraries(vfs_link_test PRIVATE engine_library_link_test_settings)
target_link_libraries(gpu_link_test PRIVATE engine_library_link_test_settings)
target_link_libraries(graphics_link_test PRIVATE engine_library_link_test_settings)
target_link_libraries(application_link_test PRIVATE engine_library_link_test_settings)
target_link_libraries(experimental_gui_link_test PRIVATE engine_library_link_test_settings)
if(NOT EMSCRIPTEN)
    target_link_libraries(experimental_async_link_test PRIVATE engine_library_link_test_settings)
endif()

include(${POMDOG_DIR}/cmake/pomdog/asan.cmake)
pomdog_copy_asan_runtime(math_link_test)
pomdog_copy_asan_runtime(random_link_test)
pomdog_copy_asan_runtime(vfs_link_test)
pomdog_copy_asan_runtime(gpu_link_test)
pomdog_copy_asan_runtime(graphics_link_test)
pomdog_copy_asan_runtime(application_link_test)
pomdog_copy_asan_runtime(experimental_gui_link_test)
if(NOT EMSCRIPTEN)
    pomdog_copy_asan_runtime(experimental_async_link_test)
endif()
