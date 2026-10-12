add_library(pomdog_experimental_i18n STATIC)
add_library(pomdog::experimental_i18n ALIAS pomdog_experimental_i18n)

target_link_libraries(pomdog_experimental_i18n
    PUBLIC
        pomdog_base
    PRIVATE
        pomdog_build_settings
)

source_group(experimental/i18n REGULAR_EXPRESSION "pomdog/experimental/i18n/*")

target_sources(pomdog_experimental_i18n PRIVATE
    ${POMDOG_SRC_DIR}/experimental/i18n/line_break_language.h
    ${POMDOG_SRC_DIR}/experimental/i18n/line_break_rules.cpp
    ${POMDOG_SRC_DIR}/experimental/i18n/line_break_rules.h
    ${POMDOG_SRC_DIR}/experimental/i18n/text_wrap.cpp
    ${POMDOG_SRC_DIR}/experimental/i18n/text_wrap.h
)
