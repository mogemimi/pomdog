#pragma once

#include "devgui_overlay/devgui_overlay.h"
#include "pomdog/experimental/skeletal2d/animation_state.h"
#include "pomdog/experimental/skeletal2d/animation_system.h"
#include "pomdog/experimental/skeletal2d/animator.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/skin.h"
#include "pomdog/experimental/skeletal2d/skinned_mesh.h"
#include "pomdog/graphics/primitive_batch.h"
#include "pomdog/graphics/sprite_batch.h"
#include "pomdog/graphics/wireframe_effect.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

using namespace pomdog;

/// AnimationGraphTest plays a skinned mesh with an animation graph. The panel
/// switches the model and the state of the graph, and sets the parameter that
/// blends two clips and the playback rate.
class AnimationGraphTest final : public Game {
public:
    explicit AnimationGraphTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    /// Loads the model at the given index of the models that the test shows.
    [[nodiscard]] std::unique_ptr<Error>
    loadModel(i32 modelIndex);

    /// Sets the parameters of the animator from the panel.
    void applyParameters();

    void buildUI(devgui::UI& ui);

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
    WireframeEffect wireframeEffect_;
    std::shared_ptr<gpu::ConstantBuffer> modelConstantBuffer_;
    std::shared_ptr<gpu::ConstantBuffer> worldConstantBuffer_;

    DevGUIOverlay overlay_;
    std::string statusText_;
    i32 modelIndex_ = 0;
    i32 stateIndex_ = 0;
    f32 playbackRate_ = 1.0f;
    f32 weight_ = 0.0f;
    bool showWireframe_ = false;
};

} // namespace feature_showcase
