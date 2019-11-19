#pragma once

#include <Pomdog/Experimental/Graphics/PrimitiveBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFont.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFontLoader.hpp>
#include <Pomdog/Experimental/UI/DrawingContext.hpp>
#include <Pomdog/Experimental/UI/UIElementHierarchy.hpp>
#include <Pomdog/Experimental/UI/StackPanel.hpp>
#include <Pomdog/Experimental/UI/Slider.hpp>
#include <Pomdog/Experimental/UI/TextBlock.hpp>
#include <Pomdog/Experimental/UI/TreeView.hpp>
#include <Pomdog/Experimental/UI/ToggleSwitch.hpp>
#include <Pomdog/Experimental/UI/DebugNavigator.hpp>
#include <Pomdog/Experimental/UI/HorizontalLayout.hpp>
#include <Pomdog/Experimental/UI/PushButton.hpp>
#include <Pomdog/Experimental/UI/TextField.hpp>
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
    
    std::unique_ptr<UI::DrawingContext> drawingContext;
    std::unique_ptr<UI::UIElementHierarchy> hierarchy;
    std::shared_ptr<UI::TextField> textField;

    std::string propertyText1;
    std::string propertyText2;
};

} // namespace FeatureShowcase
