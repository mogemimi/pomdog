#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/skeletal2d/animation_state.h"
#include "pomdog/experimental/skeletal2d/animation_system.h"
#include "pomdog/experimental/skeletal2d/animator.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/skin.h"
#include "pomdog/experimental/skeletal2d/skinned_mesh.h"
#include "pomdog/experimental/texture_packer/texture_atlas.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class AnimationGraphTest final : public Game {
public:
    explicit AnimationGraphTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::shared_ptr<gpu::Texture2D> texture;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;

    std::shared_ptr<skeletal2d::Skeleton> skeleton;
    std::shared_ptr<skeletal2d::SkeletonPose> skeletonPose;
    std::vector<Matrix3x2> globalPose;
    skeletal2d::SkinnedMesh skinnedMesh;
    std::shared_ptr<skeletal2d::Animator> animator;

    std::shared_ptr<gpu::SamplerState> sampler;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer;
    std::shared_ptr<gpu::PipelineState> pipelineState;
    std::shared_ptr<gpu::PipelineState> pipelineStateWireframe;
    std::shared_ptr<gpu::ConstantBuffer> modelConstantBuffer;
    std::shared_ptr<gpu::ConstantBuffer> worldConstantBuffer;
};

} // namespace feature_showcase
