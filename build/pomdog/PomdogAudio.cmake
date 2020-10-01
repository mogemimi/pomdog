target_sources(pomdog_static PRIVATE
    # NOTE: Audio
    ${POMDOG_INC_DIR}/Audio/AudioChannels.hpp
    ${POMDOG_INC_DIR}/Audio/AudioClip.hpp
    ${POMDOG_SRC_DIR}/Audio/AudioClip.cpp
    ${POMDOG_INC_DIR}/Audio/AudioEmitter.hpp
    ${POMDOG_INC_DIR}/Audio/AudioEngine.hpp
    ${POMDOG_SRC_DIR}/Audio/AudioEngine.cpp
    ${POMDOG_INC_DIR}/Audio/AudioListener.hpp
    ${POMDOG_INC_DIR}/Audio/SoundEffect.hpp
    ${POMDOG_SRC_DIR}/Audio/SoundEffect.cpp
    ${POMDOG_INC_DIR}/Audio/SoundState.hpp
    ${POMDOG_INC_DIR}/Audio/detail/ForwardDeclarations.hpp
)
