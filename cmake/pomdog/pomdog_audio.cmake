add_library(pomdog_audio STATIC)
add_library(pomdog::audio ALIAS pomdog_audio)

target_link_libraries(pomdog_audio
    PUBLIC
        pomdog_base
    PRIVATE
        pomdog_build_settings
        pomdog_math
        $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_XAUDIO2}>>:
            xaudio2.lib
        >
        $<$<AND:$<PLATFORM_ID:Linux,Emscripten>,$<BOOL:${POMDOG_USE_OPENAL}>>:
            openal
        >
)

source_group(audio          REGULAR_EXPRESSION "pomdog/audio/*")
source_group(audio/details  REGULAR_EXPRESSION "pomdog/audio/details/*")
source_group(audio/openal   REGULAR_EXPRESSION "pomdog/audio/openal/*")
source_group(audio/xaudio2  REGULAR_EXPRESSION "pomdog/audio/xaudio2/*")

target_sources(pomdog_audio PRIVATE
    # NOTE: audio
    ${POMDOG_SRC_DIR}/audio/audio_channels.h
    ${POMDOG_SRC_DIR}/audio/audio_clip.h
    ${POMDOG_SRC_DIR}/audio/audio_clip.cpp
    ${POMDOG_SRC_DIR}/audio/audio_engine.h
    ${POMDOG_SRC_DIR}/audio/audio_engine.cpp
    ${POMDOG_SRC_DIR}/audio/audio_source.h
    ${POMDOG_SRC_DIR}/audio/audio_source.cpp
    ${POMDOG_SRC_DIR}/audio/details/audio_clip_file.h
    ${POMDOG_SRC_DIR}/audio/details/audio_clip_file.cpp
    ${POMDOG_SRC_DIR}/audio/details/audio_clip_helpers.h
    ${POMDOG_SRC_DIR}/audio/details/audio_clip_helpers.cpp

    $<$<PLATFORM_ID:Linux,Emscripten,Darwin>:
        # NOTE: audio/openal
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_al.h
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_streaming_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_streaming_al.h
        ${POMDOG_SRC_DIR}/audio/openal/audio_engine_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_engine_al.h
        ${POMDOG_SRC_DIR}/audio/openal/audio_source_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_source_al.h
        ${POMDOG_SRC_DIR}/audio/openal/audio_source_streaming_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_source_streaming_al.h
        ${POMDOG_SRC_DIR}/audio/openal/error_checker_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/error_checker_al.h
        ${POMDOG_SRC_DIR}/audio/openal/openal_headers.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: audio/xaudio2
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_streaming_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_streaming_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_engine_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_engine_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_source_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_source_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_source_streaming_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_source_streaming_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/xaudio2_headers.h
    >
)

target_link_options(pomdog_audio INTERFACE
    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_OPENAL}>>:
        "SHELL:-framework AudioToolBox"
        "SHELL:-framework OpenAL"
    >
)
