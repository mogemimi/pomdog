#pragma once

#include <Pomdog/Experimental/Graphics/PrimitiveBatch.hpp>
#include <Pomdog/Experimental/MagicaVoxel/VoxModel.hpp>
#include <Pomdog/Experimental/MagicaVoxel/VoxModelLoader.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

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
    std::shared_ptr<MagicaVoxel::VoxModel> voxelModel;
};

} // namespace FeatureShowcase
