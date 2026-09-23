add_library(pomdog_experimental_image_effects STATIC)
add_library(pomdog::experimental_image_effects ALIAS pomdog_experimental_image_effects)

target_link_libraries(pomdog_experimental_image_effects
    PUBLIC
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_content_gpu
        pomdog_gpu
)

source_group(experimental/image_effects REGULAR_EXPRESSION "pomdog/experimental/image_effects/*")

target_sources(pomdog_experimental_image_effects PRIVATE
    ${POMDOG_SRC_DIR}/experimental/image_effects/chromatic_aberration.h
    ${POMDOG_SRC_DIR}/experimental/image_effects/chromatic_aberration.cpp
    ${POMDOG_SRC_DIR}/experimental/image_effects/fish_eye_effect.h
    ${POMDOG_SRC_DIR}/experimental/image_effects/fish_eye_effect.cpp
    ${POMDOG_SRC_DIR}/experimental/image_effects/fxaa.h
    ${POMDOG_SRC_DIR}/experimental/image_effects/fxaa.cpp
    ${POMDOG_SRC_DIR}/experimental/image_effects/grayscale_effect.h
    ${POMDOG_SRC_DIR}/experimental/image_effects/grayscale_effect.cpp
    ${POMDOG_SRC_DIR}/experimental/image_effects/image_effect_base.h
    ${POMDOG_SRC_DIR}/experimental/image_effects/image_effect_base.cpp
    ${POMDOG_SRC_DIR}/experimental/image_effects/post_process_compositor.h
    ${POMDOG_SRC_DIR}/experimental/image_effects/post_process_compositor.cpp
    ${POMDOG_SRC_DIR}/experimental/image_effects/retro_crt_effect.h
    ${POMDOG_SRC_DIR}/experimental/image_effects/retro_crt_effect.cpp
    ${POMDOG_SRC_DIR}/experimental/image_effects/screen_quad.h
    ${POMDOG_SRC_DIR}/experimental/image_effects/screen_quad.cpp
    ${POMDOG_SRC_DIR}/experimental/image_effects/sepia_tone_effect.h
    ${POMDOG_SRC_DIR}/experimental/image_effects/sepia_tone_effect.cpp
    ${POMDOG_SRC_DIR}/experimental/image_effects/vignette_effect.h
    ${POMDOG_SRC_DIR}/experimental/image_effects/vignette_effect.cpp
)
