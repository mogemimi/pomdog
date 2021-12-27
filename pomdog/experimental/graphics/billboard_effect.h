// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/content/asset_builders/builder.h"
#include "pomdog/graphics/forward_declarations.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AssetManager;
class GraphicsDevice;
class GraphicsCommandList;
class Texture2D;
class PipelineState;
class SamplerState;
class VertexBuffer;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT BillboardBatchBuffer final {
public:
    BillboardBatchBuffer(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        int capacity);

    BillboardBatchBuffer(const BillboardBatchBuffer&) = delete;
    BillboardBatchBuffer(BillboardBatchBuffer&&) = default;

    BillboardBatchBuffer& operator=(const BillboardBatchBuffer&) = delete;
    BillboardBatchBuffer& operator=(BillboardBatchBuffer&&) = default;

    ~BillboardBatchBuffer();

    void Reset();

    void AddBillboard(
        const Vector3& position,
        const Color& color,
        float scale);

    void AddBillboard(
        const Vector3& position,
        const Color& color,
        const Radian<float>& rotationZ,
        const Vector2& originPivot,
        float scale);

    void AddBillboard(
        const Vector3& position,
        const Vector2& textureCoord,
        const Vector2& textureSize,
        const Color& color,
        const Radian<float>& rotationZ,
        const Vector2& originPivot,
        const Vector2& scale);

    void FetchBuffer();

    const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const;

    /// Return the number of instances of the billboard.
    int GetSize() const noexcept;

    /// Return the number of instances that can be held in currently allocated storage.
    int GetCapacity() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

class POMDOG_EXPORT BillboardBatchEffect final {
public:
    BillboardBatchEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    BillboardBatchEffect(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        std::optional<BlendDescription>&& blendDesc,
        std::optional<DepthStencilDescription>&& depthStencilDesc,
        std::optional<RasterizerDescription>&& rasterizerDesc,
        std::optional<SurfaceFormat>&& renderTargetViewFormat,
        std::optional<SurfaceFormat>&& depthStencilViewFormat,
        AssetManager& assets);

    ~BillboardBatchEffect();

    BillboardBatchEffect(const BillboardBatchEffect&) = delete;
    BillboardBatchEffect(BillboardBatchEffect&&) = default;

    BillboardBatchEffect& operator=(const BillboardBatchEffect&) = delete;
    BillboardBatchEffect& operator=(BillboardBatchEffect&&) = default;

    void Draw(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const std::shared_ptr<Texture2D>& texture,
        const std::shared_ptr<SamplerState>& sampler,
        const std::shared_ptr<ConstantBuffer>& constantBuffer,
        std::size_t constantBufferOffset,
        const BillboardBatchBuffer& billboardInstances);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
