source_group(basic REGULAR_EXPRESSION "pomdog/basic/*")

target_sources(pomdog_static PRIVATE
    # NOTE: basic
    ${POMDOG_SRC_DIR}/basic/compilation_target_architecture.cpp
    ${POMDOG_SRC_DIR}/basic/compilation_target_architecture.h
    ${POMDOG_SRC_DIR}/basic/conditional_compilation.h
    ${POMDOG_SRC_DIR}/basic/export.h
    ${POMDOG_SRC_DIR}/basic/flatbuffers_macros.h
    ${POMDOG_SRC_DIR}/basic/platform.h
    ${POMDOG_SRC_DIR}/basic/types.cpp
    ${POMDOG_SRC_DIR}/basic/types.h
    ${POMDOG_SRC_DIR}/basic/unreachable.h
    ${POMDOG_SRC_DIR}/basic/version.h
)
