source_group(console REGULAR_EXPRESSION "pomdog/console/*")

target_sources(pomdog_static PRIVATE
    # NOTE: console
    ${POMDOG_SRC_DIR}/console/console.cpp
    ${POMDOG_SRC_DIR}/console/console.h
)
