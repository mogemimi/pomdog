add_library(pomdog_experimental_spine STATIC)
add_library(pomdog::experimental_spine ALIAS pomdog_experimental_spine)

target_link_libraries(pomdog_experimental_spine
    PUBLIC
        pomdog_math
        pomdog_experimental_skeletal2d
        pomdog_experimental_texture_atlas
    PRIVATE
        pomdog_build_settings
        pomdog_content_utility
        pomdog_vfs
)

source_group(experimental/spine REGULAR_EXPRESSION "pomdog/experimental/spine/*")

target_sources(pomdog_experimental_spine PRIVATE
    ${POMDOG_SRC_DIR}/experimental/spine/animation_graph_builder.cpp
    ${POMDOG_SRC_DIR}/experimental/spine/animation_graph_builder.h
    ${POMDOG_SRC_DIR}/experimental/spine/animation_loader.cpp
    ${POMDOG_SRC_DIR}/experimental/spine/animation_loader.h
    ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc_loader.cpp
    ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc_loader.h
    ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc.h
    ${POMDOG_SRC_DIR}/experimental/spine/skeleton_loader.cpp
    ${POMDOG_SRC_DIR}/experimental/spine/skeleton_loader.h
    ${POMDOG_SRC_DIR}/experimental/spine/skin_loader.cpp
    ${POMDOG_SRC_DIR}/experimental/spine/skin_loader.h
    ${POMDOG_SRC_DIR}/experimental/spine/skinned_mesh_loader.cpp
    ${POMDOG_SRC_DIR}/experimental/spine/skinned_mesh_loader.h
)

target_include_directories(pomdog_experimental_spine PRIVATE
    ${POMDOG_DIR}/thirdparty/rapidjson/include
)
