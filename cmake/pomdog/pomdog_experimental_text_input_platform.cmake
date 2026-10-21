add_library(pomdog_experimental_text_input_platform STATIC)
add_library(pomdog::experimental_text_input_platform ALIAS pomdog_experimental_text_input_platform)

target_link_libraries(pomdog_experimental_text_input_platform
    PUBLIC
        pomdog_application
        pomdog_experimental_text_input
    PRIVATE
        pomdog_build_settings
        pomdog_application_gpu
        pomdog_input
        $<$<PLATFORM_ID:Windows>:
            imm32.lib
        >
        $<$<AND:$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_X11}>>:
            X11
        >
        # NOTE: The X11 game window's header includes the OpenGL headers.
        $<$<AND:$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_GL4}>>:
            glew_static
        >
)

source_group(experimental/text_input            REGULAR_EXPRESSION "pomdog/experimental/text_input/*")
source_group(experimental/text_input/emscripten REGULAR_EXPRESSION "pomdog/experimental/text_input/emscripten/*")
source_group(experimental/text_input/win32      REGULAR_EXPRESSION "pomdog/experimental/text_input/win32/*")
source_group(experimental/text_input/x11        REGULAR_EXPRESSION "pomdog/experimental/text_input/x11/*")

target_sources(pomdog_experimental_text_input_platform PRIVATE
    ${POMDOG_SRC_DIR}/experimental/text_input/platform_text_input.cpp
    ${POMDOG_SRC_DIR}/experimental/text_input/platform_text_input.h

    $<$<PLATFORM_ID:Emscripten>:
        ${POMDOG_SRC_DIR}/experimental/text_input/emscripten/text_input_service_emscripten.cpp
        ${POMDOG_SRC_DIR}/experimental/text_input/emscripten/text_input_service_emscripten.h
    >

    $<$<PLATFORM_ID:Windows>:
        ${POMDOG_SRC_DIR}/experimental/text_input/win32/text_input_service_win32.cpp
        ${POMDOG_SRC_DIR}/experimental/text_input/win32/text_input_service_win32.h
    >

    $<$<AND:$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_X11}>>:
        ${POMDOG_SRC_DIR}/experimental/text_input/x11/text_input_service_x11.cpp
        ${POMDOG_SRC_DIR}/experimental/text_input/x11/text_input_service_x11.h
    >
)
