add_library(pomdog_graphics STATIC)
add_library(pomdog::graphics ALIAS pomdog_graphics)

target_link_libraries(pomdog_graphics
    PUBLIC
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_gpu
        pomdog_vfs
        pomdog_content_gpu
        stb_static
)

source_group(graphics REGULAR_EXPRESSION "pomdog/graphics/*")

target_sources(pomdog_graphics PRIVATE
    ${POMDOG_SRC_DIR}/graphics/basic_effect.h
    ${POMDOG_SRC_DIR}/graphics/basic_effect.cpp
    ${POMDOG_SRC_DIR}/graphics/billboard_effect.h
    ${POMDOG_SRC_DIR}/graphics/billboard_effect.cpp
    ${POMDOG_SRC_DIR}/graphics/font_glyph.h
    ${POMDOG_SRC_DIR}/graphics/line_batch.h
    ${POMDOG_SRC_DIR}/graphics/line_batch.cpp
    ${POMDOG_SRC_DIR}/graphics/polyline_batch.h
    ${POMDOG_SRC_DIR}/graphics/polyline_batch.cpp
    ${POMDOG_SRC_DIR}/graphics/polygon_shape_builder.h
    ${POMDOG_SRC_DIR}/graphics/polygon_shape_builder.cpp
    ${POMDOG_SRC_DIR}/graphics/primitive_batch.h
    ${POMDOG_SRC_DIR}/graphics/primitive_batch.cpp
    ${POMDOG_SRC_DIR}/graphics/sprite_batch.h
    ${POMDOG_SRC_DIR}/graphics/sprite_batch.cpp
    ${POMDOG_SRC_DIR}/graphics/sprite_font.h
    ${POMDOG_SRC_DIR}/graphics/sprite_font.cpp
    ${POMDOG_SRC_DIR}/graphics/sprite_font_sdf.h
    ${POMDOG_SRC_DIR}/graphics/sprite_font_sdf.cpp
    ${POMDOG_SRC_DIR}/graphics/sprite_line.h
    ${POMDOG_SRC_DIR}/graphics/sprite_line.cpp
    ${POMDOG_SRC_DIR}/graphics/texture_region.h
    ${POMDOG_SRC_DIR}/graphics/truetype_font.h
    ${POMDOG_SRC_DIR}/graphics/truetype_font.cpp
    ${POMDOG_SRC_DIR}/graphics/wireframe_effect.h
    ${POMDOG_SRC_DIR}/graphics/wireframe_effect.cpp
)
