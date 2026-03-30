// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class ConstantBuffer;
class GraphicsDevice;
class SamplerState;
class Texture2D;
class VertexBuffer;
struct BlendDesc;
struct DepthStencilDesc;
struct RasterizerDesc;
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
class POMDOG_EXPORT BillboardBatchBuffer {
public:
    virtual ~BillboardBatchBuffer();

    virtual void reset() = 0;

    virtual void addBillboard(
        const Vector3& position,
        const Color& color,
        f32 scale) = 0;

    virtual void addBillboard(
        const Vector3& position,
        const Color& color,
        const Radian<f32>& rotationZ,
        const Vector2& originPivot,
        f32 scale) = 0;

    virtual void addBillboard(
        const Vector3& position,
        const Vector2& textureCoord,
        const Vector2& textureSize,
        const Color& color,
        const Radian<f32>& rotationZ,
        const Vector2& originPivot,
        const Vector2& scale) = 0;

    virtual void fetchBuffer() = 0;

    [[nodiscard]] virtual const std::shared_ptr<gpu::VertexBuffer>&
    getVertexBuffer() const = 0;

    /// Return the number of instances of the billboard.
    [[nodiscard]] virtual u32
    getSize() const noexcept = 0;

    /// Return the number of instances that can be held in currently allocated storage.
    [[nodiscard]] virtual u32
    getCapacity() const noexcept = 0;
};

/// Creates a BillboardBatchBuffer instance.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<BillboardBatchBuffer>, std::unique_ptr<Error>>
createBillboardBatchBuffer(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    u32 capacity) noexcept;

/// Renders billboard instances with a pipeline state for batched drawing.
class POMDOG_EXPORT BillboardBatchEffect {
public:
    virtual ~BillboardBatchEffect();

    virtual void draw(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<gpu::Texture2D>& texture,
        const std::shared_ptr<gpu::SamplerState>& sampler,
        const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer,
        u32 constantBufferOffset,
        const BillboardBatchBuffer& billboardInstances) = 0;
};

/// Creates a BillboardBatchEffect instance.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<BillboardBatchEffect>, std::unique_ptr<Error>>
createBillboardBatchEffect(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) noexcept;

/// Creates a BillboardBatchEffect instance with custom settings.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<BillboardBatchEffect>, std::unique_ptr<Error>>
createBillboardBatchEffect(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::BlendDesc>&& blendDesc,
    std::optional<gpu::DepthStencilDesc>&& depthStencilDesc,
    std::optional<gpu::RasterizerDesc>&& rasterizerDesc,
    std::optional<gpu::PixelFormat>&& renderTargetViewFormat,
    std::optional<gpu::PixelFormat>&& depthStencilViewFormat) noexcept;

} // namespace pomdog
