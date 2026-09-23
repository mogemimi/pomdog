add_library(pomdog_content_utility STATIC)
add_library(pomdog::content_utility ALIAS pomdog_content_utility)

target_link_libraries(pomdog_content_utility
    PUBLIC pomdog_base
    PRIVATE pomdog_build_settings
)

source_group(content REGULAR_EXPRESSION "pomdog/content/*")
source_group(content/utility REGULAR_EXPRESSION "pomdog/content/utility/*")

target_sources(pomdog_content_utility PRIVATE
    ${POMDOG_SRC_DIR}/content/utility/binary_reader.h
    ${POMDOG_SRC_DIR}/content/utility/binary_reader.cpp
    ${POMDOG_SRC_DIR}/content/utility/make_fourcc.h
)
