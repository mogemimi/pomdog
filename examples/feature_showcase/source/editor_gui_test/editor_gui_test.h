#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/experimental/graphics/truetype_font_loader.h"
#include "pomdog/experimental/gui/check_box.h"
#include "pomdog/experimental/gui/context_menu.h"
#include "pomdog/experimental/gui/debug_navigator.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/float_field.h"
#include "pomdog/experimental/gui/horizontal_layout.h"
#include "pomdog/experimental/gui/int_field.h"
#include "pomdog/experimental/gui/list_view.h"
#include "pomdog/experimental/gui/popup_menu.h"
#include "pomdog/experimental/gui/push_button.h"
#include "pomdog/experimental/gui/scroll_view.h"
#include "pomdog/experimental/gui/slider.h"
#include "pomdog/experimental/gui/stack_panel.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/text_field.h"
#include "pomdog/experimental/gui/toggle_switch.h"
#include "pomdog/experimental/gui/tree_view.h"
#include "pomdog/experimental/gui/vector3_field.h"
#include "pomdog/experimental/gui/widget_hierarchy.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class EditorGUITest final : public Game {
public:
    explicit EditorGUITest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    initialize() override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;

    std::unique_ptr<gui::DrawingContext> drawingContext;
    std::unique_ptr<gui::WidgetHierarchy> hierarchy;
    std::shared_ptr<gui::TextField> textField;

    std::string propertyText1;
    std::string propertyText2;
};

} // namespace feature_showcase
