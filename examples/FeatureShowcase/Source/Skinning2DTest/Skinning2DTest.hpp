#pragma once

#include <Pomdog/Experimental/Graphics/PrimitiveBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/TexturePacker/TextureAtlas.hpp>
#include <Pomdog/Experimental/Skeletal2D/AnimationState.hpp>
#include <Pomdog/Experimental/Skeletal2D/AnimationSystem.hpp>
#include <Pomdog/Experimental/Skeletal2D/Skeleton.hpp>
#include <Pomdog/Experimental/Skeletal2D/SkeletonPose.hpp>
#include <Pomdog/Experimental/Skeletal2D/Skin.hpp>
#include <Pomdog/Experimental/Skeletal2D/SkinnedMesh.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class Skinning2DTest final : public Game {
public:
    explicit Skinning2DTest(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<Texture2D> texture;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;

    Skeletal2D::AnimationSystem animationSystem;
    std::shared_ptr<Skeletal2D::Skeleton> skeleton;
    std::shared_ptr<Skeletal2D::SkeletonPose> skeletonPose;
    std::shared_ptr<Skeletal2D::AnimationState> animationState;
    std::vector<Matrix3x2> globalPose;
    Skeletal2D::SkinnedMesh skinnedMesh;

    std::shared_ptr<SamplerState> sampler;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<PipelineState> pipelineStateWireframe;
    std::shared_ptr<ConstantBuffer> modelConstantBuffer;
    std::shared_ptr<ConstantBuffer> worldConstantBuffer;
};

} // namespace FeatureShowcase
