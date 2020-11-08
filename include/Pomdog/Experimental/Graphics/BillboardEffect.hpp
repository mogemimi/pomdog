// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetBuilders/Builder.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Math/ForwardDeclarations.hpp"
#include <memory>
#include <optional>

namespace Pomdog {
class AssetManager;
class GraphicsDevice;
class GraphicsCommandList;
class Texture2D;
class PipelineState;
class SamplerState;
class VertexBuffer;
} // namespace Pomdog

namespace Pomdog {

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
        std::optional<DepthFormat>&& depthStencilViewFormat,
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
        const BillboardBatchBuffer& billboardInstances);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
