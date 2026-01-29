// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/content/asset_builders/builder.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AssetManager;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT BillboardBatchBuffer final {
public:
    BillboardBatchBuffer(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        int capacity);

    BillboardBatchBuffer(const BillboardBatchBuffer&) = delete;
    BillboardBatchBuffer(BillboardBatchBuffer&&) = default;

    BillboardBatchBuffer& operator=(const BillboardBatchBuffer&) = delete;
    BillboardBatchBuffer& operator=(BillboardBatchBuffer&&) = default;

    ~BillboardBatchBuffer();

    void reset();

    void addBillboard(
        const Vector3& position,
        const Color& color,
        float scale);

    void addBillboard(
        const Vector3& position,
        const Color& color,
        const Radian<f32>& rotationZ,
        const Vector2& originPivot,
        float scale);

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
    [[nodiscard]] int
    getSize() const noexcept;

    /// Return the number of instances that can be held in currently allocated storage.
    [[nodiscard]] int
    getCapacity() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

class POMDOG_EXPORT BillboardBatchEffect final {
public:
    BillboardBatchEffect(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    BillboardBatchEffect(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::BlendDescriptor>&& blendDesc,
        std::optional<gpu::DepthStencilDescriptor>&& depthStencilDesc,
        std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
        std::optional<PixelFormat>&& renderTargetViewFormat,
        std::optional<PixelFormat>&& depthStencilViewFormat,
        AssetManager& assets);

    ~BillboardBatchEffect();

    BillboardBatchEffect(const BillboardBatchEffect&) = delete;
    BillboardBatchEffect(BillboardBatchEffect&&) = default;

    BillboardBatchEffect& operator=(const BillboardBatchEffect&) = delete;
    BillboardBatchEffect& operator=(BillboardBatchEffect&&) = default;

    void draw(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<gpu::Texture2D>& texture,
        const std::shared_ptr<gpu::SamplerState>& sampler,
        const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer,
        std::size_t constantBufferOffset,
        const BillboardBatchBuffer& billboardInstances);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
