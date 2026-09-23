add_library(pomdog_application STATIC)
add_library(pomdog::application ALIAS pomdog_application)

target_link_libraries(pomdog_application
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)

source_group(application             REGULAR_EXPRESSION "pomdog/application/*")
source_group(application/backends    REGULAR_EXPRESSION "pomdog/application/backends/*")

target_sources(pomdog_application PRIVATE
    ${POMDOG_SRC_DIR}/application/game.h
    ${POMDOG_SRC_DIR}/application/game.cpp
    ${POMDOG_SRC_DIR}/application/game_host.h
    ${POMDOG_SRC_DIR}/application/game_host.cpp
    ${POMDOG_SRC_DIR}/application/game_setup.h
    ${POMDOG_SRC_DIR}/application/game_setup.cpp
    ${POMDOG_SRC_DIR}/application/backends/frame_rate_limiter.h
    ${POMDOG_SRC_DIR}/application/backends/frame_rate_limiter.cpp
)
