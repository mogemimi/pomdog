target_sources(pomdog_static PRIVATE
    # NOTE: Content
    ${POMDOG_INC_DIR}/Content/AssetManager.hpp
    ${POMDOG_SRC_DIR}/Content/AssetManager.cpp
    ${POMDOG_INC_DIR}/Content/AssetBuilders/Builder.hpp
    ${POMDOG_INC_DIR}/Content/AssetBuilders/PipelineStateBuilder.hpp
    ${POMDOG_SRC_DIR}/Content/AssetBuilders/PipelineStateBuilder.cpp
    ${POMDOG_INC_DIR}/Content/AssetBuilders/ShaderBuilder.hpp
    ${POMDOG_SRC_DIR}/Content/AssetBuilders/ShaderBuilder.cpp
    ${POMDOG_INC_DIR}/Content/Audio/Vorbis.hpp
    ${POMDOG_SRC_DIR}/Content/Audio/Vorbis.cpp
    ${POMDOG_INC_DIR}/Content/Audio/WAV.hpp
    ${POMDOG_SRC_DIR}/Content/Audio/WAV.cpp
    ${POMDOG_INC_DIR}/Content/Image/ImageBuffer.hpp
    ${POMDOG_INC_DIR}/Content/Image/DDS.hpp
    ${POMDOG_SRC_DIR}/Content/Image/DDS.cpp
    ${POMDOG_INC_DIR}/Content/Image/PNG.hpp
    ${POMDOG_SRC_DIR}/Content/Image/PNG.cpp
    ${POMDOG_INC_DIR}/Content/Image/PNM.hpp
    ${POMDOG_SRC_DIR}/Content/Image/PNM.cpp
    ${POMDOG_INC_DIR}/Content/Utility/BinaryReader.hpp
    ${POMDOG_INC_DIR}/Content/Utility/MakeFourCC.hpp
    ${POMDOG_INC_DIR}/Content/AssetLoaders/AssetLoader.hpp
    ${POMDOG_INC_DIR}/Content/AssetLoaders/AudioClipLoader.hpp
    ${POMDOG_SRC_DIR}/Content/AssetLoaders/AudioClipLoader.cpp
    ${POMDOG_INC_DIR}/Content/AssetLoaders/Texture2DLoader.hpp
    ${POMDOG_SRC_DIR}/Content/AssetLoaders/Texture2DLoader.cpp
)
