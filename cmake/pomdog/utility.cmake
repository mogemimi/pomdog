source_group(utility REGULAR_EXPRESSION "pomdog/utility/*")

target_sources(pomdog_static PRIVATE
    # NOTE: utility
    ${POMDOG_SRC_DIR}/utility/assert.hpp
    ${POMDOG_SRC_DIR}/utility/crc32.cpp
    ${POMDOG_SRC_DIR}/utility/crc32.hpp
    ${POMDOG_SRC_DIR}/utility/error_helper.cpp
    ${POMDOG_SRC_DIR}/utility/error_helper.hpp
    ${POMDOG_SRC_DIR}/utility/errors.cpp
    ${POMDOG_SRC_DIR}/utility/errors.hpp
    ${POMDOG_SRC_DIR}/utility/exception.hpp
    ${POMDOG_SRC_DIR}/utility/path_helper.cpp
    ${POMDOG_SRC_DIR}/utility/path_helper.hpp
    ${POMDOG_SRC_DIR}/utility/scope_guard.cpp
    ${POMDOG_SRC_DIR}/utility/scope_guard.hpp
    ${POMDOG_SRC_DIR}/utility/spin_lock.cpp
    ${POMDOG_SRC_DIR}/utility/spin_lock.hpp
    ${POMDOG_SRC_DIR}/utility/string_helper.cpp
    ${POMDOG_SRC_DIR}/utility/string_helper.hpp
    ${POMDOG_SRC_DIR}/utility/tagged.hpp
)
