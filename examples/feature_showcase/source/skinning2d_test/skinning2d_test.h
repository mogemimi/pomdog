#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/skeletal2d/animation_state.h"
#include "pomdog/experimental/skeletal2d/animation_system.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/skin.h"
#include "pomdog/experimental/skeletal2d/skinned_mesh.h"
#include "pomdog/experimental/texture_packer/texture_atlas.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class Skinning2DTest final : public Game {
public:
    explicit Skinning2DTest(const std::shared_ptr<GameHost>& gameHost);

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

    skeletal2d::AnimationSystem animationSystem;
    std::shared_ptr<skeletal2d::Skeleton> skeleton;
    std::shared_ptr<skeletal2d::SkeletonPose> skeletonPose;
    std::shared_ptr<skeletal2d::AnimationState> animationState;
    std::vector<Matrix3x2> globalPose;
    skeletal2d::SkinnedMesh skinnedMesh;

    std::shared_ptr<SamplerState> sampler;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<PipelineState> pipelineState;
    std::shared_ptr<PipelineState> pipelineStateWireframe;
    std::shared_ptr<ConstantBuffer> modelConstantBuffer;
    std::shared_ptr<ConstantBuffer> worldConstantBuffer;
};

} // namespace feature_showcase
