#pragma once

#include <pomdog/experimental/gui/drawing_context.hpp>
#include <pomdog/experimental/gui/horizontal_splitter.hpp>
#include <pomdog/experimental/gui/push_button.hpp>
#include <pomdog/experimental/gui/widget_hierarchy.hpp>
#include <pomdog/pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class GUISplitterTest final : public Game {
public:
    explicit GUISplitterTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::unique_ptr<GUI::DrawingContext> drawingContext;
    std::unique_ptr<GUI::WidgetHierarchy> hierarchy;
};

} // namespace FeatureShowcase
