add_library(pomdog_experimental_magicavoxel STATIC)
add_library(pomdog::experimental_magicavoxel ALIAS pomdog_experimental_magicavoxel)

target_link_libraries(pomdog_experimental_magicavoxel
    PUBLIC
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_content_utility
        pomdog_vfs
)

source_group(experimental/magicavoxel REGULAR_EXPRESSION "pomdog/experimental/magicavoxel/*")

target_sources(pomdog_experimental_magicavoxel PRIVATE
    ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model.h
    ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_exporter.h
    ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_exporter.cpp
    ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_loader.h
    ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_loader.cpp
    ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_chunk_header.h
)
