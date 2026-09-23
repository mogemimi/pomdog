add_library(pomdog_experimental_texture_atlas STATIC)
add_library(pomdog::experimental_texture_atlas ALIAS pomdog_experimental_texture_atlas)

target_link_libraries(pomdog_experimental_texture_atlas
    PUBLIC
        pomdog_graphics
    PRIVATE
        pomdog_build_settings
        pomdog_content_gpu
        pomdog_experimental_image
        pomdog_gpu
        pomdog_vfs
)

source_group(experimental/texture_atlas REGULAR_EXPRESSION "pomdog/experimental/texture_atlas/*")

target_sources(pomdog_experimental_texture_atlas PRIVATE
    ${POMDOG_SRC_DIR}/experimental/texture_atlas/texture_atlas.cpp
    ${POMDOG_SRC_DIR}/experimental/texture_atlas/texture_atlas.h
    ${POMDOG_SRC_DIR}/experimental/texture_atlas/texture_atlas_dynamic.cpp
    ${POMDOG_SRC_DIR}/experimental/texture_atlas/texture_atlas_dynamic.h
)
