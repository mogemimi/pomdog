// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class ConstantBuffer;
class GraphicsDevice;
class SamplerState;
class Texture2D;
class VertexBuffer;
struct BlendDescriptor;
struct DepthStencilDescriptor;
struct RasterizerDescriptor;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {
class Color;
class Error;
class Vector2;
class Vector3;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog::gpu {
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog {

/// Manages a buffer of billboard instances for batched rendering.
class POMDOG_EXPORT BillboardBatchBuffer final {
public:
    BillboardBatchBuffer();

    BillboardBatchBuffer(const BillboardBatchBuffer&) = delete;
    BillboardBatchBuffer(BillboardBatchBuffer&&) = default;

    BillboardBatchBuffer& operator=(const BillboardBatchBuffer&) = delete;
    BillboardBatchBuffer& operator=(BillboardBatchBuffer&&) = default;

    ~BillboardBatchBuffer();

    /// Initializes the billboard buffer by creating GPU resources.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        u32 capacity);

    void reset();

    void addBillboard(
        const Vector3& position,
        const Color& color,
        f32 scale);

    void addBillboard(
        const Vector3& position,
        const Color& color,
        const Radian<f32>& rotationZ,
        const Vector2& originPivot,
        f32 scale);

    void addBillboard(
        const Vector3& position,
        const Vector2& textureCoord,
        const Vector2& textureSize,
        const Color& color,
        const Radian<f32>& rotationZ,
        const Vector2& originPivot,
        const Vector2& scale);

    void fetchBuffer();

    [[nodiscard]] const std::shared_ptr<gpu::VertexBuffer>&
    getVertexBuffer() const;

    /// Return the number of instances of the billboard.
    [[nodiscard]] u32
    getSize() const noexcept;

    /// Return the number of instances that can be held in currently allocated storage.
    [[nodiscard]] u32
    getCapacity() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

/// Renders billboard instances with a pipeline state for batched drawing.
class POMDOG_EXPORT BillboardBatchEffect final {
public:
    BillboardBatchEffect();

    ~BillboardBatchEffect();

    BillboardBatchEffect(const BillboardBatchEffect&) = delete;
    BillboardBatchEffect(BillboardBatchEffect&&) = default;

    BillboardBatchEffect& operator=(const BillboardBatchEffect&) = delete;
    BillboardBatchEffect& operator=(BillboardBatchEffect&&) = default;

    /// Initializes the billboard effect by loading shaders and creating GPU resources.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    /// Initializes the billboard effect with custom blend, depth-stencil, rasterizer, and format settings.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::BlendDescriptor>&& blendDesc,
        std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
        std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
        std::optional<gpu::PixelFormat>&& renderTargetViewFormat,
        std::optional<gpu::PixelFormat>&& depthStencilViewFormat);

    void draw(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<gpu::Texture2D>& texture,
        const std::shared_ptr<gpu::SamplerState>& sampler,
        const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer,
        u32 constantBufferOffset,
        const BillboardBatchBuffer& billboardInstances);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
