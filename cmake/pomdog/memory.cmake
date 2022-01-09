source_group(memory REGULAR_EXPRESSION "pomdog/memory/*")

target_sources(pomdog_static PRIVATE
    # NOTE: memory
    ${POMDOG_SRC_DIR}/memory/aligned_new.h
    ${POMDOG_SRC_DIR}/memory/alignment.cpp
    ${POMDOG_SRC_DIR}/memory/alignment.h
    ${POMDOG_SRC_DIR}/memory/unsafe_ptr.h
)
