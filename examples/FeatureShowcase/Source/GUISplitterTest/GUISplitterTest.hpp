#pragma once

#include <Pomdog/Experimental/GUI/DrawingContext.hpp>
#include <Pomdog/Experimental/GUI/HorizontalSplitter.hpp>
#include <Pomdog/Experimental/GUI/PushButton.hpp>
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
