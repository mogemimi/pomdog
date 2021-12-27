source_group(content                REGULAR_EXPRESSION "pomdog/content/*")
source_group(content/asset_builders REGULAR_EXPRESSION "pomdog/content/asset_builders/*")
source_group(content/asset_loaders  REGULAR_EXPRESSION "pomdog/content/asset_loaders/*")
source_group(content/audio          REGULAR_EXPRESSION "pomdog/content/audio/*")
source_group(content/image          REGULAR_EXPRESSION "pomdog/content/image/*")
source_group(content/utility        REGULAR_EXPRESSION "pomdog/content/utility/*")

target_sources(pomdog_static PRIVATE
    # NOTE: content
    ${POMDOG_SRC_DIR}/content/asset_manager.h
    ${POMDOG_SRC_DIR}/content/asset_manager.cpp
    ${POMDOG_SRC_DIR}/content/asset_builders/builder.h
    ${POMDOG_SRC_DIR}/content/asset_builders/pipeline_state_builder.h
    ${POMDOG_SRC_DIR}/content/asset_builders/pipeline_state_builder.cpp
    ${POMDOG_SRC_DIR}/content/asset_builders/shader_builder.h
    ${POMDOG_SRC_DIR}/content/asset_builders/shader_builder.cpp
    ${POMDOG_SRC_DIR}/content/asset_loaders/asset_loader.h
    ${POMDOG_SRC_DIR}/content/asset_loaders/audio_clip_loader.h
    ${POMDOG_SRC_DIR}/content/asset_loaders/audio_clip_loader.cpp
    ${POMDOG_SRC_DIR}/content/asset_loaders/texture2d_loader.h
    ${POMDOG_SRC_DIR}/content/asset_loaders/texture2d_loader.cpp
    ${POMDOG_SRC_DIR}/content/audio/vorbis.h
    ${POMDOG_SRC_DIR}/content/audio/vorbis.cpp
    ${POMDOG_SRC_DIR}/content/audio/wav.h
    ${POMDOG_SRC_DIR}/content/audio/wav.cpp
    ${POMDOG_SRC_DIR}/content/image/image_buffer.h
    ${POMDOG_SRC_DIR}/content/image/dds.h
    ${POMDOG_SRC_DIR}/content/image/dds.cpp
    ${POMDOG_SRC_DIR}/content/image/png.h
    ${POMDOG_SRC_DIR}/content/image/png.cpp
    ${POMDOG_SRC_DIR}/content/image/pnm.h
    ${POMDOG_SRC_DIR}/content/image/pnm.cpp
    ${POMDOG_SRC_DIR}/content/utility/binary_reader.h
    ${POMDOG_SRC_DIR}/content/utility/make_fourcc.h
)
