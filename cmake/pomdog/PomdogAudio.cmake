source_group(audio          REGULAR_EXPRESSION "pomdog/audio/*")
source_group(audio\\openal  REGULAR_EXPRESSION "pomdog/audio/openal/*")
source_group(audio\\xaudio2 REGULAR_EXPRESSION "pomdog/audio/xaudio2/*")

target_sources(pomdog_static PRIVATE
    # NOTE: audio
    ${POMDOG_SRC_DIR}/audio/audio_channels.hpp
    ${POMDOG_SRC_DIR}/audio/audio_clip.hpp
    ${POMDOG_SRC_DIR}/audio/audio_clip.cpp
    ${POMDOG_SRC_DIR}/audio/audio_emitter.hpp
    ${POMDOG_SRC_DIR}/audio/audio_engine.hpp
    ${POMDOG_SRC_DIR}/audio/audio_engine.cpp
    ${POMDOG_SRC_DIR}/audio/audio_helper.hpp
    ${POMDOG_SRC_DIR}/audio/audio_helper.cpp
    ${POMDOG_SRC_DIR}/audio/audio_listener.hpp
    ${POMDOG_SRC_DIR}/audio/forward_declarations.hpp
    ${POMDOG_SRC_DIR}/audio/sound_effect.hpp
    ${POMDOG_SRC_DIR}/audio/sound_effect.cpp
    ${POMDOG_SRC_DIR}/audio/sound_state.hpp

    $<$<PLATFORM_ID:Linux,Darwin>:
        # NOTE: audio/openal
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_clip_al.hpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_engine_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/audio_engine_al.hpp
        ${POMDOG_SRC_DIR}/audio/openal/error_checker_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/error_checker_al.hpp
        ${POMDOG_SRC_DIR}/audio/openal/prerequisites_openal.hpp
        ${POMDOG_SRC_DIR}/audio/openal/sound_effect_al.cpp
        ${POMDOG_SRC_DIR}/audio/openal/sound_effect_al.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: audio/xaudio2
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_clip_xaudio2.hpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_engine_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/audio_engine_xaudio2.hpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/prerequisites_xaudio2.hpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/sound_effect_xaudio2.cpp
        ${POMDOG_SRC_DIR}/audio/xaudio2/sound_effect_xaudio2.hpp
    >
)
