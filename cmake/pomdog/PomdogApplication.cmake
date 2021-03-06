source_group(Application REGULAR_EXPRESSION "(include/Pomdog|src)/Application/*")

target_sources(pomdog_static PRIVATE
    # NOTE: Application
    ${POMDOG_INC_DIR}/Application/Duration.hpp
    ${POMDOG_INC_DIR}/Application/FileSystem.hpp
    ${POMDOG_SRC_DIR}/Application/FileSystem.cpp
    ${POMDOG_INC_DIR}/Application/Game.hpp
    ${POMDOG_SRC_DIR}/Application/Game.cpp
    ${POMDOG_INC_DIR}/Application/GameClock.hpp
    ${POMDOG_SRC_DIR}/Application/GameClock.cpp
    ${POMDOG_INC_DIR}/Application/GameHost.hpp
    ${POMDOG_SRC_DIR}/Application/GameHost.cpp
    ${POMDOG_INC_DIR}/Application/GameWindow.hpp
    ${POMDOG_SRC_DIR}/Application/GameWindow.cpp
    ${POMDOG_INC_DIR}/Application/MouseCursor.hpp
    ${POMDOG_SRC_DIR}/Application/SubsystemScheduler.hpp
    ${POMDOG_SRC_DIR}/Application/SystemEvents.hpp
    ${POMDOG_INC_DIR}/Application/TimePoint.hpp
    ${POMDOG_INC_DIR}/Application/Timer.hpp
    ${POMDOG_SRC_DIR}/Application/Timer.cpp
    ${POMDOG_SRC_DIR}/Application/TimeSource.hpp
)
