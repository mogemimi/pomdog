source_group(memory REGULAR_EXPRESSION "pomdog/memory/*")

target_sources(pomdog_static PRIVATE
    # NOTE: memory
    ${POMDOG_SRC_DIR}/memory/aligned_new.h
)
