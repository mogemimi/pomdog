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
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<RenderTarget2D> renderTarget;
    std::shared_ptr<DepthStencilBuffer> depthStencilBuffer;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    PostProcessCompositor postProcessCompositor;
};

} // namespace feature_showcase
