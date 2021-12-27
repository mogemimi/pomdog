#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/magicavoxel/vox_model.h"
#include "pomdog/experimental/magicavoxel/vox_model_loader.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class VoxelModelTest final : public Game {
public:
    explicit VoxelModelTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    std::shared_ptr<magicavoxel::VoxModel> voxelModel;
};

} // namespace feature_showcase
