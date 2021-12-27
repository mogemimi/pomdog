source_group(audio          REGULAR_EXPRESSION "pomdog/audio/*")
source_group(audio/openal   REGULAR_EXPRESSION "pomdog/audio/openal/*")
source_group(audio/xaudio2  REGULAR_EXPRESSION "pomdog/audio/xaudio2/*")

target_sources(pomdog_static PRIVATE
    # NOTE: audio
    ${POMDOG_SRC_DIR}/audio/audio_channels.h
    ${POMDOG_SRC_DIR}/audio/audio_clip.h
    ${POMDOG_SRC_DIR}/audio/audio_clip.cpp
    ${POMDOG_SRC_DIR}/audio/audio_emitter.h
    ${POMDOG_SRC_DIR}/audio/audio_engine.h
    ${POMDOG_SRC_DIR}/audio/audio_engine.cpp
    ${POMDOG_SRC_DIR}/audio/audio_helper.h
    ${POMDOG_SRC_DIR}/audio/audio_helper.cpp
    ${POMDOG_SRC_DIR}/audio/audio_listener.h
    ${POMDOG_SRC_DIR}/audio/forward_declarations.h
    ${POMDOG_SRC_DIR}/audio/sound_effect.h
    ${POMDOG_SRC_DIR}/audio/sound_effect.cpp
    ${POMDOG_SRC_DIR}/audio/sound_state.h

    $<$<PLATFORM_ID:Linux,Darwin>:
        # NOTE: audio/openal
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_al.h
        ${POMDOG_SRC_DIR}/audio/openal/audio_engine_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_engine_al.h
        ${POMDOG_SRC_DIR}/audio/openal/error_checker_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/error_checker_al.h
        ${POMDOG_SRC_DIR}/audio/openal/prerequisites_openal.h
        ${POMDOG_SRC_DIR}/audio/openal/sound_effect_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/sound_effect_al.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: audio/xaudio2
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_engine_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_engine_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/prerequisites_xaudio2.h
        ${POMDOG_SRC_DIR}/audio/xaudio2/sound_effect_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/sound_effect_xaudio2.h
    >
)
