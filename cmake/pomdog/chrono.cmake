source_group(chrono             REGULAR_EXPRESSION "pomdog/chrono/*")
source_group(chrono/apple       REGULAR_EXPRESSION "pomdog/chrono/apple/*")
source_group(chrono/detail      REGULAR_EXPRESSION "pomdog/chrono/detail/*")
source_group(chrono/emscripten  REGULAR_EXPRESSION "pomdog/chrono/emscripten/*")
source_group(chrono/linux       REGULAR_EXPRESSION "pomdog/chrono/linux/*")
source_group(chrono/win32       REGULAR_EXPRESSION "pomdog/chrono/win32/*")

target_sources(pomdog_static PRIVATE
    # NOTE: chrono
    ${POMDOG_SRC_DIR}/chrono/duration.hpp
    ${POMDOG_SRC_DIR}/chrono/game_clock.cpp
    ${POMDOG_SRC_DIR}/chrono/game_clock.hpp
    ${POMDOG_SRC_DIR}/chrono/time_point.hpp
    ${POMDOG_SRC_DIR}/chrono/time_source.cpp
    ${POMDOG_SRC_DIR}/chrono/time_source.hpp
    ${POMDOG_SRC_DIR}/chrono/timer.hpp
    ${POMDOG_SRC_DIR}/chrono/timer.cpp
    ${POMDOG_SRC_DIR}/chrono/detail/game_clock_impl.cpp
    ${POMDOG_SRC_DIR}/chrono/detail/game_clock_impl.hpp
    ${POMDOG_SRC_DIR}/chrono/detail/make_time_source.cpp
    ${POMDOG_SRC_DIR}/chrono/detail/make_time_source.hpp

    $<$<PLATFORM_ID:Darwin,iOS>:
        # NOTE: chrono/apple
        ${POMDOG_SRC_DIR}/chrono/apple/time_source_apple.cpp
        ${POMDOG_SRC_DIR}/chrono/apple/time_source_apple.hpp
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: chrono/emscripten
        ${POMDOG_SRC_DIR}/chrono/emscripten/time_source_emscripten.cpp
        ${POMDOG_SRC_DIR}/chrono/emscripten/time_source_emscripten.hpp
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: chrono/linux
        ${POMDOG_SRC_DIR}/chrono/linux/time_source_linux.cpp
        ${POMDOG_SRC_DIR}/chrono/linux/time_source_linux.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: chrono/win32
        ${POMDOG_SRC_DIR}/chrono/win32/time_source_win32.cpp
        ${POMDOG_SRC_DIR}/chrono/win32/time_source_win32.hpp
    >
)
