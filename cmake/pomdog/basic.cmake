source_group(basic REGULAR_EXPRESSION "pomdog/basic/*")

target_sources(pomdog_static PRIVATE
    # NOTE: basic
    ${POMDOG_SRC_DIR}/basic/conditional_compilation.hpp
    ${POMDOG_SRC_DIR}/basic/export.hpp
    ${POMDOG_SRC_DIR}/basic/platform.hpp
    ${POMDOG_SRC_DIR}/basic/compilation_targets.hpp
    ${POMDOG_SRC_DIR}/basic/unreachable.hpp
    ${POMDOG_SRC_DIR}/basic/version.hpp
)
