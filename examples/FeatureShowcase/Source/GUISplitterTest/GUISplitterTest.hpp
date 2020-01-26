#pragma once

#include <Pomdog/Experimental/GUI/CheckBox.hpp>
#include <Pomdog/Experimental/GUI/ContextMenu.hpp>
#include <Pomdog/Experimental/GUI/DebugNavigator.hpp>
#include <Pomdog/Experimental/GUI/DrawingContext.hpp>
#include <Pomdog/Experimental/GUI/FloatField.hpp>
#include <Pomdog/Experimental/GUI/HorizontalSplitter.hpp>
#include <Pomdog/Experimental/GUI/IntField.hpp>
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
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class GUISplitterTest final : public Game {
public:
    explicit GUISplitterTest(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;

    std::unique_ptr<GUI::DrawingContext> drawingContext;
    std::unique_ptr<GUI::WidgetHierarchy> hierarchy;
};

} // namespace FeatureShowcase
