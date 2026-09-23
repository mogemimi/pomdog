add_library(pomdog_random STATIC)
add_library(pomdog::random ALIAS pomdog_random)

target_link_libraries(pomdog_random
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)

source_group(random REGULAR_EXPRESSION "pomdog/random/*")

target_sources(pomdog_random PRIVATE
    # NOTE: random
    ${POMDOG_SRC_DIR}/random/uniform_int_distribution.h
    ${POMDOG_SRC_DIR}/random/xoroshiro128_star_star.h
    ${POMDOG_SRC_DIR}/random/xoroshiro128_star_star.cpp
)
