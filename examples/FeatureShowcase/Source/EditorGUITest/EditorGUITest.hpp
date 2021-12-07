#pragma once

#include <pomdog/experimental/gui/check_box.hpp>
#include <pomdog/experimental/gui/context_menu.hpp>
#include <pomdog/experimental/gui/debug_navigator.hpp>
#include <pomdog/experimental/gui/drawing_context.hpp>
#include <pomdog/experimental/gui/float_field.hpp>
#include <pomdog/experimental/gui/horizontal_layout.hpp>
#include <pomdog/experimental/gui/int_field.hpp>
#include <pomdog/experimental/gui/list_view.hpp>
#include <pomdog/experimental/gui/popup_menu.hpp>
#include <pomdog/experimental/gui/push_button.hpp>
#include <pomdog/experimental/gui/scroll_view.hpp>
#include <pomdog/experimental/gui/slider.hpp>
#include <pomdog/experimental/gui/stack_panel.hpp>
#include <pomdog/experimental/gui/text_block.hpp>
#include <pomdog/experimental/gui/text_field.hpp>
#include <pomdog/experimental/gui/toggle_switch.hpp>
#include <pomdog/experimental/gui/tree_view.hpp>
#include <pomdog/experimental/gui/vector3_field.hpp>
#include <pomdog/experimental/gui/widget_hierarchy.hpp>
#include <pomdog/experimental/graphics/primitive_batch.hpp>
#include <pomdog/experimental/graphics/sprite_batch.hpp>
#include <pomdog/experimental/graphics/sprite_font.hpp>
#include <pomdog/experimental/graphics/truetype_font.hpp>
#include <pomdog/experimental/graphics/truetype_font_loader.hpp>
#include <pomdog/pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class EditorGUITest final : public Game {
public:
    explicit EditorGUITest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;

    std::unique_ptr<GUI::DrawingContext> drawingContext;
    std::unique_ptr<GUI::WidgetHierarchy> hierarchy;
    std::shared_ptr<GUI::TextField> textField;

    std::string propertyText1;
    std::string propertyText2;
};

} // namespace FeatureShowcase
