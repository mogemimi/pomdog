source_group(memory REGULAR_EXPRESSION "pomdog/memory/*")

target_sources(pomdog_static PRIVATE
    # NOTE: memory
    ${POMDOG_SRC_DIR}/memory/aligned_alloc.cpp
    ${POMDOG_SRC_DIR}/memory/aligned_alloc.h
    ${POMDOG_SRC_DIR}/memory/aligned_new.h
    ${POMDOG_SRC_DIR}/memory/alignment.cpp
    ${POMDOG_SRC_DIR}/memory/alignment.h
    ${POMDOG_SRC_DIR}/memory/linear_allocator.cpp
    ${POMDOG_SRC_DIR}/memory/linear_allocator.h
    ${POMDOG_SRC_DIR}/memory/linear_page_allocator.cpp
    ${POMDOG_SRC_DIR}/memory/linear_page_allocator.h
    ${POMDOG_SRC_DIR}/memory/memcpy_span.h
    ${POMDOG_SRC_DIR}/memory/memset_span.h
    ${POMDOG_SRC_DIR}/memory/placement_new.h
    ${POMDOG_SRC_DIR}/memory/raw_ptr.h
    ${POMDOG_SRC_DIR}/memory/unsafe_ptr.h
)
