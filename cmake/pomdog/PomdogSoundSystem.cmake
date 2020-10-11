target_sources(pomdog_static PRIVATE
    $<$<PLATFORM_ID:Linux,Darwin>:
        # NOTE: SoundSystem.OpenAL
        ${POMDOG_SRC_DIR}/SoundSystem.OpenAL/AudioClipAL.cpp
        ${POMDOG_SRC_DIR}/SoundSystem.OpenAL/AudioClipAL.hpp
        ${POMDOG_SRC_DIR}/SoundSystem.OpenAL/AudioEngineAL.cpp
        ${POMDOG_SRC_DIR}/SoundSystem.OpenAL/AudioEngineAL.hpp
        ${POMDOG_SRC_DIR}/SoundSystem.OpenAL/ErrorCheckerAL.cpp
        ${POMDOG_SRC_DIR}/SoundSystem.OpenAL/ErrorCheckerAL.hpp
        ${POMDOG_SRC_DIR}/SoundSystem.OpenAL/PrerequisitesOpenAL.hpp
        ${POMDOG_SRC_DIR}/SoundSystem.OpenAL/SoundEffectAL.cpp
        ${POMDOG_SRC_DIR}/SoundSystem.OpenAL/SoundEffectAL.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: SoundSystem.XAudio2
        ${POMDOG_SRC_DIR}/SoundSystem.XAudio2/AudioClipXAudio2.cpp
        ${POMDOG_SRC_DIR}/SoundSystem.XAudio2/AudioClipXAudio2.hpp
        ${POMDOG_SRC_DIR}/SoundSystem.XAudio2/AudioEngineXAudio2.cpp
        ${POMDOG_SRC_DIR}/SoundSystem.XAudio2/AudioEngineXAudio2.hpp
        ${POMDOG_SRC_DIR}/SoundSystem.XAudio2/PrerequisitesXAudio2.hpp
        ${POMDOG_SRC_DIR}/SoundSystem.XAudio2/SoundEffectXAudio2.cpp
        ${POMDOG_SRC_DIR}/SoundSystem.XAudio2/SoundEffectXAudio2.hpp
    >
)
