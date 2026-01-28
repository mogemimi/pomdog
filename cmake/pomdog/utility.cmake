source_group(utility REGULAR_EXPRESSION "pomdog/utility/*")

target_sources(pomdog_static PRIVATE
    # NOTE: utility
    ${POMDOG_SRC_DIR}/utility/assert.h
    ${POMDOG_SRC_DIR}/utility/crc32.cpp
    ${POMDOG_SRC_DIR}/utility/crc32.h
    ${POMDOG_SRC_DIR}/utility/error_helper.cpp
    ${POMDOG_SRC_DIR}/utility/error_helper.h
    ${POMDOG_SRC_DIR}/utility/errors.cpp
    ${POMDOG_SRC_DIR}/utility/errors.h
    ${POMDOG_SRC_DIR}/utility/exception.h
    ${POMDOG_SRC_DIR}/utility/path_helper.cpp
    ${POMDOG_SRC_DIR}/utility/path_helper.h
    ${POMDOG_SRC_DIR}/utility/scope_guard.cpp
    ${POMDOG_SRC_DIR}/utility/scope_guard.h
    ${POMDOG_SRC_DIR}/utility/spin_lock.cpp
    ${POMDOG_SRC_DIR}/utility/spin_lock.h
    ${POMDOG_SRC_DIR}/utility/static_down_cast.h
    ${POMDOG_SRC_DIR}/utility/string_helper.cpp
    ${POMDOG_SRC_DIR}/utility/string_helper.h
    ${POMDOG_SRC_DIR}/utility/tagged.h
    ${POMDOG_SRC_DIR}/utility/utfcpp_headers.h
)
