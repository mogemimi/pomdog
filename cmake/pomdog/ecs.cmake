source_group(ecs REGULAR_EXPRESSION "pomdog/ecs/*")

target_sources(pomdog_static PRIVATE
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
