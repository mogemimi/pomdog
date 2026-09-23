add_library(pomdog_experimental_image STATIC)
add_library(pomdog::experimental_image ALIAS pomdog_experimental_image)

target_link_libraries(pomdog_experimental_image
    PUBLIC
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_content_gpu
        pomdog_gpu
        pomdog_vfs
)

source_group(experimental/image REGULAR_EXPRESSION "pomdog/experimental/image/*")

target_sources(pomdog_experimental_image PRIVATE
    ${POMDOG_SRC_DIR}/experimental/image/image.h
    ${POMDOG_SRC_DIR}/experimental/image/image.cpp
    ${POMDOG_SRC_DIR}/experimental/image/svg_loader.h
    ${POMDOG_SRC_DIR}/experimental/image/svg_loader.cpp
)
