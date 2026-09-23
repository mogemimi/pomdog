add_library(pomdog_content_input STATIC)
add_library(pomdog::content_input ALIAS pomdog_content_input)

target_link_libraries(pomdog_content_input
    PUBLIC
        pomdog_base
    PRIVATE
        pomdog_build_settings
        pomdog_vfs
        pomdog_content_utility
        pomdog_input
)

target_sources(pomdog_content_input PRIVATE
    ${POMDOG_SRC_DIR}/content/game_controller_db_loader.h
    ${POMDOG_SRC_DIR}/content/game_controller_db_loader.cpp
)
