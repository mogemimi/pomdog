source_group(application REGULAR_EXPRESSION "pomdog/application/*")

target_sources(pomdog_static PRIVATE
    # NOTE: application
    ${POMDOG_SRC_DIR}/application/file_system.hpp
    ${POMDOG_SRC_DIR}/application/file_system.cpp
    ${POMDOG_SRC_DIR}/application/game.hpp
    ${POMDOG_SRC_DIR}/application/game.cpp
    ${POMDOG_SRC_DIR}/application/game_host.hpp
    ${POMDOG_SRC_DIR}/application/game_host.cpp
    ${POMDOG_SRC_DIR}/application/game_window.hpp
    ${POMDOG_SRC_DIR}/application/game_window.cpp
    ${POMDOG_SRC_DIR}/application/mouse_cursor.hpp
    ${POMDOG_SRC_DIR}/application/subsystem_scheduler.hpp
    ${POMDOG_SRC_DIR}/application/system_events.hpp
)
