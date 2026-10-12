add_library(pomdog_experimental_devgui STATIC)
add_library(pomdog::experimental_devgui ALIAS pomdog_experimental_devgui)

target_link_libraries(pomdog_experimental_devgui
    PUBLIC
        pomdog_base
        pomdog_ecs
        pomdog_experimental_i18n
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_input
)

source_group(experimental/devgui REGULAR_EXPRESSION "pomdog/experimental/devgui/*")

target_sources(pomdog_experimental_devgui PRIVATE
    ${POMDOG_SRC_DIR}/experimental/devgui/antialiasing.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/antialiasing.h
    ${POMDOG_SRC_DIR}/experimental/devgui/button.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/button.h
    ${POMDOG_SRC_DIR}/experimental/devgui/checkbox.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/checkbox.h
    ${POMDOG_SRC_DIR}/experimental/devgui/color_edit.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/color_edit.h
    ${POMDOG_SRC_DIR}/experimental/devgui/combo.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/combo.h
    ${POMDOG_SRC_DIR}/experimental/devgui/context.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/context.h
    ${POMDOG_SRC_DIR}/experimental/devgui/curve_editor.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/curve_editor.h
    ${POMDOG_SRC_DIR}/experimental/devgui/devgui.h
    ${POMDOG_SRC_DIR}/experimental/devgui/dock_space.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/dock_space.h
    ${POMDOG_SRC_DIR}/experimental/devgui/drag.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/drag.h
    ${POMDOG_SRC_DIR}/experimental/devgui/draw_list.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/draw_list.h
    ${POMDOG_SRC_DIR}/experimental/devgui/horizontal_scope.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/horizontal_scope.h
    ${POMDOG_SRC_DIR}/experimental/devgui/id_scope.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/id_scope.h
    ${POMDOG_SRC_DIR}/experimental/devgui/input.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/input.h
    ${POMDOG_SRC_DIR}/experimental/devgui/input_text.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/input_text.h
    ${POMDOG_SRC_DIR}/experimental/devgui/key.h
    ${POMDOG_SRC_DIR}/experimental/devgui/list_view.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/list_view.h
    ${POMDOG_SRC_DIR}/experimental/devgui/menu.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/menu.h
    ${POMDOG_SRC_DIR}/experimental/devgui/response.h
    ${POMDOG_SRC_DIR}/experimental/devgui/scroll_area.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/scroll_area.h
    ${POMDOG_SRC_DIR}/experimental/devgui/separator.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/separator.h
    ${POMDOG_SRC_DIR}/experimental/devgui/slider.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/slider.h
    ${POMDOG_SRC_DIR}/experimental/devgui/style.h
    ${POMDOG_SRC_DIR}/experimental/devgui/text.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/text.h
    ${POMDOG_SRC_DIR}/experimental/devgui/text_edit_state.h
    ${POMDOG_SRC_DIR}/experimental/devgui/text_measurer.h
    ${POMDOG_SRC_DIR}/experimental/devgui/text_utility.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/text_utility.h
    ${POMDOG_SRC_DIR}/experimental/devgui/tooltip.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/tooltip.h
    ${POMDOG_SRC_DIR}/experimental/devgui/tree_node.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/tree_node.h
    ${POMDOG_SRC_DIR}/experimental/devgui/ui.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/ui.h
    ${POMDOG_SRC_DIR}/experimental/devgui/widget_helpers.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/widget_helpers.h
    ${POMDOG_SRC_DIR}/experimental/devgui/widget_state_storage.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/widget_state_storage.h
    ${POMDOG_SRC_DIR}/experimental/devgui/window.cpp
    ${POMDOG_SRC_DIR}/experimental/devgui/window.h
)
