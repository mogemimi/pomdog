source_group(Async REGULAR_EXPRESSION "(include/Pomdog|src)/Async/*")

target_sources(pomdog_static PRIVATE
    # NOTE: Async
    ${POMDOG_INC_DIR}/Async/Helpers.hpp
    ${POMDOG_INC_DIR}/Async/ImmediateScheduler.hpp
    ${POMDOG_SRC_DIR}/Async/ImmediateScheduler.cpp
    ${POMDOG_INC_DIR}/Async/QueuedScheduler.hpp
    ${POMDOG_SRC_DIR}/Async/QueuedScheduler.cpp
    ${POMDOG_INC_DIR}/Async/Scheduler.hpp
    ${POMDOG_SRC_DIR}/Async/Scheduler.cpp
    ${POMDOG_INC_DIR}/Async/Task.hpp
    ${POMDOG_SRC_DIR}/Async/Task.cpp
)
