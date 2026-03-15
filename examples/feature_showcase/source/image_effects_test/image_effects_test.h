#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/widget_hierarchy.h"
#include "pomdog/experimental/image_effects/image_effect_base.h"
#include "pomdog/experimental/image_effects/post_process_compositor.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

using namespace pomdog;

class ImageEffectsTest final : public Game {
public:
    explicit ImageEffectsTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost, int argc, const char* const* argv) override;

    void update() override;

    void draw() override;

private:
    void rebuildComposite();

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<gpu::RenderTarget2D> renderTarget;
    std::shared_ptr<gpu::DepthStencilBuffer> depthStencilBuffer;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    PostProcessCompositor postProcessCompositor;

    std::unique_ptr<gui::DrawingContext> drawingContext_;
    std::unique_ptr<gui::WidgetHierarchy> hierarchy_;

    struct EffectEntry final {
        std::string name;
        std::shared_ptr<ImageEffectBase> effect;
        bool enabled = false;
    };
    std::vector<EffectEntry> effectEntries_;
};

} // namespace feature_showcase
