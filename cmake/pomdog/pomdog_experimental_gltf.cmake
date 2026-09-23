add_library(pomdog_experimental_gltf STATIC)
add_library(pomdog::experimental_gltf ALIAS pomdog_experimental_gltf)

target_link_libraries(pomdog_experimental_gltf
    PUBLIC
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_content_utility
        pomdog_filesystem
        pomdog_vfs
)

source_group(experimental/gltf REGULAR_EXPRESSION "pomdog/experimental/gltf/*")

target_sources(pomdog_experimental_gltf PRIVATE
    ${POMDOG_SRC_DIR}/experimental/gltf/gltf.h
    ${POMDOG_SRC_DIR}/experimental/gltf/gltf.cpp
    ${POMDOG_SRC_DIR}/experimental/gltf/gltf_loader.h
    ${POMDOG_SRC_DIR}/experimental/gltf/gltf_loader.cpp
)

target_include_directories(pomdog_experimental_gltf PRIVATE
    ${POMDOG_DIR}/thirdparty/rapidjson/include
)
