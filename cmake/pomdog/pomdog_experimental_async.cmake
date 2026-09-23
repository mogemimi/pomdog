add_library(pomdog_experimental_async STATIC)
add_library(pomdog::experimental_async ALIAS pomdog_experimental_async)

target_link_libraries(pomdog_experimental_async
    PUBLIC
        pomdog_base
    PRIVATE
        pomdog_build_settings
)

source_group(experimental/async REGULAR_EXPRESSION "pomdog/experimental/async/*")

target_sources(pomdog_experimental_async PRIVATE
    ${POMDOG_SRC_DIR}/experimental/async/helpers.h
    ${POMDOG_SRC_DIR}/experimental/async/immediate_scheduler.h
    ${POMDOG_SRC_DIR}/experimental/async/immediate_scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/queued_scheduler.h
    ${POMDOG_SRC_DIR}/experimental/async/queued_scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/scheduler.h
    ${POMDOG_SRC_DIR}/experimental/async/scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/task.h
    ${POMDOG_SRC_DIR}/experimental/async/task.cpp
)
