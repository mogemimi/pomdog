// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class ConstantBuffer;
class GraphicsDevice;
class PipelineState;
class VertexBuffer;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {
class Color;
class Error;
class Vector3;
} // namespace pomdog

namespace pomdog {

/// WireframeEffect renders a debug wireframe overlay by expanding a triangle
/// list to individual line segments on the CPU and drawing them with
/// LineList topology. This avoids reliance on FillMode::Wireframe, which is
/// unsupported on WebGL/Emscripten.
class POMDOG_EXPORT WireframeEffect final {
private:
    std::shared_ptr<gpu::PipelineState> pipelineState_;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;
    u32 vertexCount_ = 0;

public:
    WireframeEffect();

    /// Initializes the wireframe pipeline state using the built-in
    /// position+color shader variant. Uses the presentation parameters from
    /// graphicsDevice to determine the default render target and depth-stencil
    /// pixel formats.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    /// Converts a triangle-list mesh to line segments and uploads to the GPU.
    /// indexCount must be a multiple of 3. getVertex is called with each raw
    /// vertex index (as stored in the index buffer) and must return the
    /// corresponding world-space position. Accepts any index type because the
    /// caller provides the lookup via the callback.
    /// Call each frame when vertex positions change (e.g., animated meshes).
    void setTriangleMesh(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const Color& color,
        u32 indexCount,
        std::function<Vector3(u32 index)> getVertex);

    /// Draws the wireframe lines using the given constant buffers.
    /// Binds modelConstantBuffer to slot 0 and worldConstantBuffer to slot 1,
    /// matching the layout expected by basic_effect_position_color shaders.
    void draw(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<gpu::ConstantBuffer>& modelConstantBuffer,
        const std::shared_ptr<gpu::ConstantBuffer>& worldConstantBuffer);
};

} // namespace pomdog
