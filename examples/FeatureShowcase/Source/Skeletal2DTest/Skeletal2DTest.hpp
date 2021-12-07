#pragma once

#include <pomdog/experimental/graphics/primitive_batch.hpp>
#include <pomdog/experimental/graphics/sprite_batch.hpp>
#include <pomdog/experimental/skeletal2d/animation_state.hpp>
#include <pomdog/experimental/skeletal2d/animation_system.hpp>
#include <pomdog/experimental/skeletal2d/skeleton.hpp>
#include <pomdog/experimental/skeletal2d/skeleton_pose.hpp>
#include <pomdog/experimental/skeletal2d/skin.hpp>
#include <pomdog/experimental/texture_packer/texture_atlas.hpp>
#include <pomdog/pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class Skeletal2DTest final : public Game {
public:
    explicit Skeletal2DTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::shared_ptr<Texture2D> texture;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;

    Skeletal2D::AnimationSystem animationSystem;
    std::shared_ptr<Skeletal2D::Skeleton> skeleton;
    std::shared_ptr<Skeletal2D::SkeletonPose> skeletonPose;
    std::shared_ptr<Skeletal2D::AnimationState> animationState;
    std::shared_ptr<Skeletal2D::Skin> skin;
    std::vector<Matrix3x2> globalPose;

    std::shared_ptr<SamplerState> sampler;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<PipelineState> pipelineStateWireframe;
    std::shared_ptr<ConstantBuffer> modelConstantBuffer;
    std::shared_ptr<ConstantBuffer> worldConstantBuffer;
};

} // namespace FeatureShowcase
