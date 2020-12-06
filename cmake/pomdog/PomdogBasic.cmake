source_group(Basic REGULAR_EXPRESSION "(include/Pomdog|src)/Basic/*")

target_sources(pomdog_static PRIVATE
    # NOTE: Basic
    ${POMDOG_SRC_DIR}/Basic/ConditionalCompilation.hpp
    ${POMDOG_INC_DIR}/Basic/Export.hpp
    ${POMDOG_INC_DIR}/Basic/Platform.hpp
    ${POMDOG_SRC_DIR}/Basic/Unreachable.hpp
    ${POMDOG_INC_DIR}/Basic/Version.hpp
)
