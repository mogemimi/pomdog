add_library(pomdog_content_gpu STATIC)
add_library(pomdog::content_gpu ALIAS pomdog_content_gpu)

target_link_libraries(pomdog_content_gpu
    PUBLIC
        pomdog_gpu
    PRIVATE
        pomdog_build_settings
        pomdog_vfs
        pomdog_content_utility
        pomdog_math
        png
)

source_group(content/image REGULAR_EXPRESSION "pomdog/content/image/*")

target_sources(pomdog_content_gpu PRIVATE
    ${POMDOG_SRC_DIR}/content/shader_loader.h
    ${POMDOG_SRC_DIR}/content/shader_loader.cpp
    ${POMDOG_SRC_DIR}/content/texture_loader.h
    ${POMDOG_SRC_DIR}/content/texture_loader.cpp
    ${POMDOG_SRC_DIR}/content/image/image_container.h
    ${POMDOG_SRC_DIR}/content/image/dds.h
    ${POMDOG_SRC_DIR}/content/image/dds.cpp
    ${POMDOG_SRC_DIR}/content/image/png.h
    ${POMDOG_SRC_DIR}/content/image/png.cpp
    ${POMDOG_SRC_DIR}/content/image/pnm.h
    ${POMDOG_SRC_DIR}/content/image/pnm.cpp
)
