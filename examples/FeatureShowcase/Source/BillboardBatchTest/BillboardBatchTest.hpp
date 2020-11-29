#pragma once

#include <Pomdog/Experimental/Graphics/BillboardEffect.hpp>
#include <Pomdog/Experimental/Graphics/LineBatch.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class BillboardBatchTest final : public Game {
public:
    explicit BillboardBatchTest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::shared_ptr<Error>
    Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<ConstantBuffer> constantBuffer;
    std::shared_ptr<Texture2D> texture;
    std::shared_ptr<SamplerState> sampler;
    std::shared_ptr<BillboardBatchBuffer> billboardBuffer;
    std::shared_ptr<BillboardBatchEffect> billboardEffect;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<LineBatch> lineBatch;
};

} // namespace FeatureShowcase
