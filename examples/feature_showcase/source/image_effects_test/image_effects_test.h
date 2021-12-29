#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/image_effects/post_process_compositor.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class ImageEffectsTest final : public Game {
public:
    explicit ImageEffectsTest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<gpu::RenderTarget2D> renderTarget;
    std::shared_ptr<gpu::DepthStencilBuffer> depthStencilBuffer;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    PostProcessCompositor postProcessCompositor;
};

} // namespace feature_showcase
