#pragma once

#include "devgui_overlay/devgui_overlay.h"
#include "pomdog/experimental/mesh3d/mesh_part.h"
#include "pomdog/graphics/basic_effect.h"
#include "pomdog/graphics/wireframe_effect.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

/// Mesh3DTest draws the meshes that the asset pipeline converted to `.mesh3d`
/// files: a mesh of a glTF binary file and a mesh of a MagicaVoxel file. The
/// panel switches the mesh, turns it, and shows it with its normals as colors
/// or as a wireframe, alone or over the faces.
class Mesh3DTest final : public Game {
public:
    explicit Mesh3DTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    /// DisplayMode chooses how the mesh appears.
    enum class DisplayMode : i32 {
        Solid = 0,
        Normals = 1,
        SolidWithWireframe = 2,
        Wireframe = 3,
    };

    /// MeshDesc tells where a mesh is and how to draw it.
    struct MeshDesc final {
        std::string_view meshFilePath;
        std::string_view verticesFilePath;
        std::string_view indicesFilePath;
        std::string_view meshName;
        BasicEffect::BasicEffectVariant variant;
        Matrix4x4 baseMatrix;
    };

    /// LoadedMesh holds the GPU resources of a mesh.
    struct LoadedMesh final {
        std::vector<mesh3d::MeshPart> parts;
        std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
        std::shared_ptr<gpu::VertexBuffer> normalVertexBuffer;
        std::shared_ptr<gpu::IndexBuffer> indexBuffer;
        std::shared_ptr<gpu::PipelineState> pipelineState;
        WireframeEffect wireframeEffect;
        WireframeEffect overlayWireframeEffect;
        Matrix4x4 baseMatrix = Matrix4x4::createIdentity();
        u32 vertexCount = 0;
        u32 triangleCount = 0;
        bool textured = false;
    };

    [[nodiscard]] std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
    createPipelineState(BasicEffect::BasicEffectVariant variant);

    [[nodiscard]] std::unique_ptr<Error>
    loadMesh(const MeshDesc& desc, LoadedMesh& mesh);

    void buildUI(devgui::UI& ui);

    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    ConnectionList connect_;

    std::vector<LoadedMesh> meshes_;
    std::shared_ptr<gpu::PipelineState> normalPipelineState_;
    std::shared_ptr<gpu::SamplerState> sampler_;
    std::shared_ptr<gpu::ConstantBuffer> modelConstantBuffer_;
    std::shared_ptr<gpu::ConstantBuffer> worldConstantBuffer_;
    std::shared_ptr<gpu::Texture2D> texture_;

    DevGUIOverlay overlay_;
    i32 meshIndex_ = 0;
    i32 displayMode_ = static_cast<i32>(DisplayMode::Solid);
    f32 yawDegrees_ = 0.0f;
    f32 pitchDegrees_ = 0.0f;
    bool autoRotate_ = true;
};

} // namespace feature_showcase
