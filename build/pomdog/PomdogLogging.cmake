target_sources(pomdog_static PRIVATE
    # NOTE: Logging
    ${POMDOG_INC_DIR}/Logging/Log.hpp
    ${POMDOG_SRC_DIR}/Logging/Log.cpp
    ${POMDOG_INC_DIR}/Logging/LogChannel.hpp
    ${POMDOG_SRC_DIR}/Logging/LogChannel.cpp
    ${POMDOG_INC_DIR}/Logging/LogEntry.hpp
    ${POMDOG_INC_DIR}/Logging/LogLevel.hpp
)
