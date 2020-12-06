source_group(Audio          REGULAR_EXPRESSION "(include/Pomdog|src)/Audio/*")
source_group(Audio.OpenAL   REGULAR_EXPRESSION "(include/Pomdog|src)/Audio.OpenAL/*")
source_group(Audio.XAudio2  REGULAR_EXPRESSION "(include/Pomdog|src)/Audio.XAudio2/*")

target_sources(pomdog_static PRIVATE
    # NOTE: Audio
    ${POMDOG_INC_DIR}/Audio/AudioChannels.hpp
    ${POMDOG_INC_DIR}/Audio/AudioClip.hpp
    ${POMDOG_SRC_DIR}/Audio/AudioClip.cpp
    ${POMDOG_INC_DIR}/Audio/AudioEmitter.hpp
    ${POMDOG_INC_DIR}/Audio/AudioEngine.hpp
    ${POMDOG_SRC_DIR}/Audio/AudioEngine.cpp
    ${POMDOG_SRC_DIR}/Audio/AudioHelper.hpp
    ${POMDOG_SRC_DIR}/Audio/AudioHelper.cpp
    ${POMDOG_INC_DIR}/Audio/AudioListener.hpp
    ${POMDOG_INC_DIR}/Audio/ForwardDeclarations.hpp
    ${POMDOG_INC_DIR}/Audio/SoundEffect.hpp
    ${POMDOG_SRC_DIR}/Audio/SoundEffect.cpp
    ${POMDOG_INC_DIR}/Audio/SoundState.hpp

    $<$<PLATFORM_ID:Linux,Darwin>:
        # NOTE: Audio.OpenAL
        ${POMDOG_SRC_DIR}/Audio.OpenAL/AudioClipAL.cpp
        ${POMDOG_SRC_DIR}/Audio.OpenAL/AudioClipAL.hpp
        ${POMDOG_SRC_DIR}/Audio.OpenAL/AudioEngineAL.cpp
        ${POMDOG_SRC_DIR}/Audio.OpenAL/AudioEngineAL.hpp
        ${POMDOG_SRC_DIR}/Audio.OpenAL/ErrorCheckerAL.cpp
        ${POMDOG_SRC_DIR}/Audio.OpenAL/ErrorCheckerAL.hpp
        ${POMDOG_SRC_DIR}/Audio.OpenAL/PrerequisitesOpenAL.hpp
        ${POMDOG_SRC_DIR}/Audio.OpenAL/SoundEffectAL.cpp
        ${POMDOG_SRC_DIR}/Audio.OpenAL/SoundEffectAL.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: Audio.XAudio2
        ${POMDOG_SRC_DIR}/Audio.XAudio2/AudioClipXAudio2.cpp
        ${POMDOG_SRC_DIR}/Audio.XAudio2/AudioClipXAudio2.hpp
        ${POMDOG_SRC_DIR}/Audio.XAudio2/AudioEngineXAudio2.cpp
        ${POMDOG_SRC_DIR}/Audio.XAudio2/AudioEngineXAudio2.hpp
        ${POMDOG_SRC_DIR}/Audio.XAudio2/PrerequisitesXAudio2.hpp
        ${POMDOG_SRC_DIR}/Audio.XAudio2/SoundEffectXAudio2.cpp
        ${POMDOG_SRC_DIR}/Audio.XAudio2/SoundEffectXAudio2.hpp
    >
)
