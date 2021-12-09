source_group(logging REGULAR_EXPRESSION "pomdog/logging/*")

target_sources(pomdog_static PRIVATE
    # NOTE: logging
    ${POMDOG_SRC_DIR}/logging/log.hpp
    ${POMDOG_SRC_DIR}/logging/log.cpp
    ${POMDOG_SRC_DIR}/logging/log_channel.hpp
    ${POMDOG_SRC_DIR}/logging/log_channel.cpp
    ${POMDOG_SRC_DIR}/logging/log_entry.hpp
    ${POMDOG_SRC_DIR}/logging/log_level.hpp
)
