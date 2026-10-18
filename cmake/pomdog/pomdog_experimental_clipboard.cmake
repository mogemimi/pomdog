add_library(pomdog_experimental_clipboard STATIC)
add_library(pomdog::experimental_clipboard ALIAS pomdog_experimental_clipboard)

target_link_libraries(pomdog_experimental_clipboard
    PUBLIC
        pomdog_base
    PRIVATE
        pomdog_build_settings
)

source_group(experimental/clipboard REGULAR_EXPRESSION "pomdog/experimental/clipboard/*")

target_sources(pomdog_experimental_clipboard PRIVATE
    ${POMDOG_SRC_DIR}/experimental/clipboard/clipboard_capabilities.h
    ${POMDOG_SRC_DIR}/experimental/clipboard/clipboard_event.h
    ${POMDOG_SRC_DIR}/experimental/clipboard/clipboard_service.cpp
    ${POMDOG_SRC_DIR}/experimental/clipboard/clipboard_service.h
    ${POMDOG_SRC_DIR}/experimental/clipboard/clipboard_service_local.cpp
    ${POMDOG_SRC_DIR}/experimental/clipboard/clipboard_service_local.h
    ${POMDOG_SRC_DIR}/experimental/clipboard/line_breaks.cpp
    ${POMDOG_SRC_DIR}/experimental/clipboard/line_breaks.h
)
