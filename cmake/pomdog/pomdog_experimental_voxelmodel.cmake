add_library(pomdog_experimental_voxelmodel STATIC)
add_library(pomdog::experimental_voxelmodel ALIAS pomdog_experimental_voxelmodel)

target_link_libraries(pomdog_experimental_voxelmodel
    PUBLIC
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_vfs
)

source_group(experimental/voxelmodel REGULAR_EXPRESSION "pomdog/experimental/voxelmodel/*")

target_sources(pomdog_experimental_voxelmodel PRIVATE
    ${POMDOG_SRC_DIR}/experimental/voxelmodel/voxel_model.cpp
    ${POMDOG_SRC_DIR}/experimental/voxelmodel/voxel_model.h
    ${POMDOG_SRC_DIR}/experimental/voxelmodel/voxel_model_loader.cpp
    ${POMDOG_SRC_DIR}/experimental/voxelmodel/voxel_model_loader.h
    ${POMDOG_SRC_DIR}/experimental/voxelmodel/voxel_model_writer.cpp
    ${POMDOG_SRC_DIR}/experimental/voxelmodel/voxel_model_writer.h
)
