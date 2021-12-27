source_group(logging REGULAR_EXPRESSION "pomdog/logging/*")

target_sources(pomdog_static PRIVATE
    # NOTE: logging
    ${POMDOG_SRC_DIR}/logging/log.h
    ${POMDOG_SRC_DIR}/logging/log.cpp
    ${POMDOG_SRC_DIR}/logging/log_channel.h
    ${POMDOG_SRC_DIR}/logging/log_channel.cpp
    ${POMDOG_SRC_DIR}/logging/log_entry.h
    ${POMDOG_SRC_DIR}/logging/log_level.h
)
