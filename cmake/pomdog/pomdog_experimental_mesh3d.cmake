add_library(pomdog_experimental_mesh3d STATIC)
add_library(pomdog::experimental_mesh3d ALIAS pomdog_experimental_mesh3d)

target_link_libraries(pomdog_experimental_mesh3d
    PUBLIC
        pomdog_gpu
    PRIVATE
        pomdog_build_settings
        pomdog_vfs
)

source_group(experimental/mesh3d REGULAR_EXPRESSION "pomdog/experimental/mesh3d/*")

target_sources(pomdog_experimental_mesh3d PRIVATE
    ${POMDOG_SRC_DIR}/experimental/mesh3d/mesh_buffers.cpp
    ${POMDOG_SRC_DIR}/experimental/mesh3d/mesh_buffers.h
    ${POMDOG_SRC_DIR}/experimental/mesh3d/mesh_data.cpp
    ${POMDOG_SRC_DIR}/experimental/mesh3d/mesh_data.h
    ${POMDOG_SRC_DIR}/experimental/mesh3d/mesh_part.h
    ${POMDOG_SRC_DIR}/experimental/mesh3d/vertex_layout.h
)
