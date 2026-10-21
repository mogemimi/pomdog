add_library(pomdog_experimental_text_input STATIC)
add_library(pomdog::experimental_text_input ALIAS pomdog_experimental_text_input)

target_link_libraries(pomdog_experimental_text_input
    PUBLIC
        pomdog_base
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_input
)

source_group(experimental/text_input REGULAR_EXPRESSION "pomdog/experimental/text_input/*")

target_sources(pomdog_experimental_text_input PRIVATE
    ${POMDOG_SRC_DIR}/experimental/text_input/composition_buffer.cpp
    ${POMDOG_SRC_DIR}/experimental/text_input/composition_buffer.h
    ${POMDOG_SRC_DIR}/experimental/text_input/software_keyboard_state.h
    ${POMDOG_SRC_DIR}/experimental/text_input/text_field_state.h
    ${POMDOG_SRC_DIR}/experimental/text_input/text_input_capabilities.h
    ${POMDOG_SRC_DIR}/experimental/text_input/text_input_event.h
    ${POMDOG_SRC_DIR}/experimental/text_input/text_input_service.cpp
    ${POMDOG_SRC_DIR}/experimental/text_input/text_input_service.h
    ${POMDOG_SRC_DIR}/experimental/text_input/text_input_service_keyboard.cpp
    ${POMDOG_SRC_DIR}/experimental/text_input/text_input_service_keyboard.h
    ${POMDOG_SRC_DIR}/experimental/text_input/utf16_conversion.cpp
    ${POMDOG_SRC_DIR}/experimental/text_input/utf16_conversion.h
    ${POMDOG_SRC_DIR}/experimental/text_input/utf32_conversion.cpp
    ${POMDOG_SRC_DIR}/experimental/text_input/utf32_conversion.h
)
