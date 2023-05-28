#pragma once

#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/horizontal_splitter.h"
#include "pomdog/experimental/gui/push_button.h"
#include "pomdog/experimental/gui/widget_hierarchy.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class GUISplitterTest final : public Game {
public:
    explicit GUISplitterTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::unique_ptr<gui::DrawingContext> drawingContext;
    std::unique_ptr<gui::WidgetHierarchy> hierarchy;
};

} // namespace feature_showcase
