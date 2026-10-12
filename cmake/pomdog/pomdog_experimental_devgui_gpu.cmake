add_library(pomdog_experimental_devgui_gpu STATIC)
add_library(pomdog::experimental_devgui_gpu ALIAS pomdog_experimental_devgui_gpu)

target_link_libraries(pomdog_experimental_devgui_gpu
    PUBLIC
        pomdog_experimental_devgui
    PRIVATE
        pomdog_build_settings
        pomdog_gpu
        pomdog_graphics
        pomdog_vfs
)

source_group(experimental/devgui REGULAR_EXPRESSION "pomdog/experimental/devgui/*")

target_sources(pomdog_experimental_devgui_gpu PRIVATE
    ${POMDOG_SRC_DIR}/experimental/devgui/renderer.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/renderer.h
)
