add_library(pomdog_console STATIC)
add_library(pomdog::console ALIAS pomdog_console)

target_link_libraries(pomdog_console
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)

source_group(console REGULAR_EXPRESSION "pomdog/console/*")

target_sources(pomdog_console PRIVATE
    # NOTE: console
    ${POMDOG_SRC_DIR}/console/console.cpp
    ${POMDOG_SRC_DIR}/console/console.h
)
