source_group(Content                REGULAR_EXPRESSION "(include/Pomdog|src)/Content/*")
source_group(Content\\AssetBuilders REGULAR_EXPRESSION "(include/Pomdog|src)/Content/AssetBuilders/*")
source_group(Content\\AssetLoaders  REGULAR_EXPRESSION "(include/Pomdog|src)/Content/AssetLoaders/*")
source_group(Content\\Audio         REGULAR_EXPRESSION "(include/Pomdog|src)/Content/Audio/*")
source_group(Content\\Image         REGULAR_EXPRESSION "(include/Pomdog|src)/Content/Image/*")
source_group(Content\\Utility       REGULAR_EXPRESSION "(include/Pomdog|src)/Content/Utility/*")

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
