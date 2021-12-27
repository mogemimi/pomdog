source_group(basic REGULAR_EXPRESSION "pomdog/basic/*")

target_sources(pomdog_static PRIVATE
    # NOTE: basic
    ${POMDOG_SRC_DIR}/basic/conditional_compilation.h
    ${POMDOG_SRC_DIR}/basic/export.h
    ${POMDOG_SRC_DIR}/basic/platform.h
    ${POMDOG_SRC_DIR}/basic/compilation_targets.h
    ${POMDOG_SRC_DIR}/basic/unreachable.h
    ${POMDOG_SRC_DIR}/basic/version.h
)
