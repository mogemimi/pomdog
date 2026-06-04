source_group(random REGULAR_EXPRESSION "pomdog/random/*")

target_sources(pomdog_static PRIVATE
    # NOTE: random
    ${POMDOG_SRC_DIR}/random/uniform_int_distribution.h
    ${POMDOG_SRC_DIR}/random/xoroshiro128_star_star.h
    ${POMDOG_SRC_DIR}/random/xoroshiro128_star_star.cpp
)
