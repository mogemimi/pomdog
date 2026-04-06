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
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class AnimationGraphTest final : public Game {
public:
    explicit AnimationGraphTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost, int argc, const char* const* argv) override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    ConnectionList connect_;

    std::shared_ptr<gpu::Texture2D> texture_;
    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;

    std::shared_ptr<skeletal2d::Skeleton> skeleton_;
    std::shared_ptr<skeletal2d::SkeletonPose> skeletonPose_;
    std::vector<Matrix3x2> globalPose_;
    skeletal2d::SkinnedMesh skinnedMesh_;
    std::shared_ptr<skeletal2d::Animator> animator_;

    std::shared_ptr<gpu::SamplerState> sampler_;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;
    std::shared_ptr<gpu::IndexBuffer> indexBuffer_;
    std::shared_ptr<gpu::PipelineState> pipelineState_;
    std::shared_ptr<gpu::PipelineState> pipelineStateWireframe_;
    std::shared_ptr<gpu::ConstantBuffer> modelConstantBuffer_;
    std::shared_ptr<gpu::ConstantBuffer> worldConstantBuffer_;
};

} // namespace feature_showcase
