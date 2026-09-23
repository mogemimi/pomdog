add_library(pomdog_content_audio STATIC)
add_library(pomdog::content_audio ALIAS pomdog_content_audio)

target_link_libraries(pomdog_content_audio
    PUBLIC
        pomdog_audio
    PRIVATE
        pomdog_build_settings
        pomdog_vfs
        pomdog_content_utility
        stb_static
)

source_group(content/audio REGULAR_EXPRESSION "pomdog/content/audio/*")

target_sources(pomdog_content_audio PRIVATE
    ${POMDOG_SRC_DIR}/content/audio_clip_loader.h
    ${POMDOG_SRC_DIR}/content/audio_clip_loader.cpp
    ${POMDOG_SRC_DIR}/content/audio/audio_container.h
    ${POMDOG_SRC_DIR}/content/audio/audio_container.cpp
    ${POMDOG_SRC_DIR}/content/audio/vorbis.h
    ${POMDOG_SRC_DIR}/content/audio/vorbis.cpp
    ${POMDOG_SRC_DIR}/content/audio/wav.h
    ${POMDOG_SRC_DIR}/content/audio/wav.cpp
)
