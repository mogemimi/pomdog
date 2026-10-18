add_library(pomdog_experimental_clipboard_platform STATIC)
add_library(pomdog::experimental_clipboard_platform ALIAS pomdog_experimental_clipboard_platform)

target_link_libraries(pomdog_experimental_clipboard_platform
    PUBLIC
        pomdog_application
        pomdog_experimental_clipboard
    PRIVATE
        pomdog_build_settings
        pomdog_application_gpu
        $<$<PLATFORM_ID:Windows>:
            user32.lib
        >
)

source_group(experimental/clipboard            REGULAR_EXPRESSION "pomdog/experimental/clipboard/*")
source_group(experimental/clipboard/emscripten REGULAR_EXPRESSION "pomdog/experimental/clipboard/emscripten/*")
source_group(experimental/clipboard/win32      REGULAR_EXPRESSION "pomdog/experimental/clipboard/win32/*")

target_sources(pomdog_experimental_clipboard_platform PRIVATE
    ${POMDOG_SRC_DIR}/experimental/clipboard/platform_clipboard.cpp
    ${POMDOG_SRC_DIR}/experimental/clipboard/platform_clipboard.h

    $<$<PLATFORM_ID:Emscripten>:
        ${POMDOG_SRC_DIR}/experimental/clipboard/emscripten/clipboard_service_emscripten.cpp
        ${POMDOG_SRC_DIR}/experimental/clipboard/emscripten/clipboard_service_emscripten.h
    >

    $<$<PLATFORM_ID:Windows>:
        ${POMDOG_SRC_DIR}/experimental/clipboard/win32/clipboard_service_win32.cpp
        ${POMDOG_SRC_DIR}/experimental/clipboard/win32/clipboard_service_win32.h
    >
)
