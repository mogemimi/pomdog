source_group(filesystem            REGULAR_EXPRESSION "pomdog/filesystem/*")
source_group(filesystem/apple      REGULAR_EXPRESSION "pomdog/filesystem/apple/*")
source_group(filesystem/emscripten REGULAR_EXPRESSION "pomdog/filesystem/emscripten/*")
source_group(filesystem/linux      REGULAR_EXPRESSION "pomdog/filesystem/linux/*")
source_group(filesystem/win32      REGULAR_EXPRESSION "pomdog/filesystem/win32/*")

target_sources(pomdog_static PRIVATE
    # NOTE: application
    ${POMDOG_SRC_DIR}/filesystem/file_system.hpp
    ${POMDOG_SRC_DIR}/filesystem/file_system.cpp

    $<$<PLATFORM_ID:Darwin,iOS>:
        # NOTE: filesystem/apple
        ${POMDOG_SRC_DIR}/filesystem/apple/file_system_apple.hpp
        ${POMDOG_SRC_DIR}/filesystem/apple/file_system_apple.mm
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: filesystem/emscripten
        ${POMDOG_SRC_DIR}/filesystem/emscripten/file_system_emscripten.cpp
        ${POMDOG_SRC_DIR}/filesystem/emscripten/file_system_emscripten.hpp
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: filesystem/linux
        ${POMDOG_SRC_DIR}/filesystem/linux/file_system_linux.cpp
        ${POMDOG_SRC_DIR}/filesystem/linux/file_system_linux.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: filesystem/win32
        ${POMDOG_SRC_DIR}/filesystem/win32/file_system_win32.cpp
        ${POMDOG_SRC_DIR}/filesystem/win32/file_system_win32.hpp
    >
)
