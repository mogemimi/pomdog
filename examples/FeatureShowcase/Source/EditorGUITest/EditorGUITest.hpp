#pragma once

#include <Pomdog/Experimental/GUI/CheckBox.hpp>
#include <Pomdog/Experimental/GUI/ContextMenu.hpp>
#include <Pomdog/Experimental/GUI/DebugNavigator.hpp>
#include <Pomdog/Experimental/GUI/DrawingContext.hpp>
#include <Pomdog/Experimental/GUI/FloatField.hpp>
#include <Pomdog/Experimental/GUI/HorizontalLayout.hpp>
#include <Pomdog/Experimental/GUI/IntField.hpp>
#include <Pomdog/Experimental/GUI/ListView.hpp>
#include <Pomdog/Experimental/GUI/PopupMenu.hpp>
#include <Pomdog/Experimental/GUI/PushButton.hpp>
#include <Pomdog/Experimental/GUI/ScrollView.hpp>
#include <Pomdog/Experimental/GUI/Slider.hpp>
#include <Pomdog/Experimental/GUI/StackPanel.hpp>
#include <Pomdog/Experimental/GUI/TextBlock.hpp>
#include <Pomdog/Experimental/GUI/TextField.hpp>
#include <Pomdog/Experimental/GUI/ToggleSwitch.hpp>
#include <Pomdog/Experimental/GUI/TreeView.hpp>
#include <Pomdog/Experimental/GUI/Vector3Field.hpp>
#include <Pomdog/Experimental/GUI/WidgetHierarchy.hpp>
#include <Pomdog/Experimental/Graphics/PrimitiveBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFontLoader.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class EditorGUITest final : public Game {
public:
    explicit EditorGUITest(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

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
