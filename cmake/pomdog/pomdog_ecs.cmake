add_library(pomdog_ecs STATIC)
add_library(pomdog::ecs ALIAS pomdog_ecs)

target_link_libraries(pomdog_ecs
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)

source_group(ecs REGULAR_EXPRESSION "pomdog/ecs/*")

target_sources(pomdog_ecs PRIVATE
    # NOTE: ecs
    ${POMDOG_SRC_DIR}/ecs/entity_bin.cpp
    ${POMDOG_SRC_DIR}/ecs/entity_bin.h
    ${POMDOG_SRC_DIR}/ecs/entity_lru_queue.cpp
    ${POMDOG_SRC_DIR}/ecs/entity_lru_queue.h
    ${POMDOG_SRC_DIR}/ecs/entity_pool.cpp
    ${POMDOG_SRC_DIR}/ecs/entity_pool.h
    ${POMDOG_SRC_DIR}/ecs/entity.cpp
    ${POMDOG_SRC_DIR}/ecs/entity.h
    ${POMDOG_SRC_DIR}/ecs/tagged_entity.cpp
    ${POMDOG_SRC_DIR}/ecs/tagged_entity.h
)
