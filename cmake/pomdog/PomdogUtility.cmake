source_group(Utility REGULAR_EXPRESSION "(include/Pomdog|src)/Utility/*")

target_sources(pomdog_static PRIVATE
    # NOTE: Utility
    ${POMDOG_SRC_DIR}/Utility/AlignedNew.hpp
    ${POMDOG_INC_DIR}/Utility/Assert.hpp
    ${POMDOG_INC_DIR}/Utility/Detail/CRC32.hpp
    ${POMDOG_SRC_DIR}/Utility/CRC32.cpp
    ${POMDOG_SRC_DIR}/Utility/ErrorHelper.hpp
    ${POMDOG_SRC_DIR}/Utility/ErrorHelper.cpp
    ${POMDOG_INC_DIR}/Utility/Errors.hpp
    ${POMDOG_SRC_DIR}/Utility/Errors.cpp
    ${POMDOG_SRC_DIR}/Utility/Exception.hpp
    ${POMDOG_INC_DIR}/Utility/PathHelper.hpp
    ${POMDOG_SRC_DIR}/Utility/PathHelper.cpp
    ${POMDOG_SRC_DIR}/Utility/ScopeGuard.hpp
    ${POMDOG_SRC_DIR}/Utility/ScopeGuard.cpp
    ${POMDOG_INC_DIR}/Utility/Detail/SpinLock.hpp
    ${POMDOG_SRC_DIR}/Utility/SpinLock.cpp
    ${POMDOG_INC_DIR}/Utility/StringHelper.hpp
    ${POMDOG_SRC_DIR}/Utility/StringHelper.cpp
    ${POMDOG_SRC_DIR}/Utility/Tagged.hpp
)
