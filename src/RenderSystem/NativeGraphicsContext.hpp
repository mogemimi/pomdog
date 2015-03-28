// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVEGRAPHICSCONTEXT_D292A72E_HPP
#define POMDOG_NATIVEGRAPHICSCONTEXT_D292A72E_HPP

#include "../Utility/Noncopyable.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <vector>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class GraphicsCapabilities;
class NativeEffectPass;
class NativeConstantLayout;

class NativeGraphicsContext: Noncopyable {
public:
    virtual ~NativeGraphicsContext() = default;

    virtual void Clear(Color const& color) = 0;

    virtual void Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil) = 0;

    virtual void Present() = 0;

    virtual void Draw(PrimitiveTopology primitiveTopology, std::size_t vertexCount) = 0;

    virtual void DrawIndexed(PrimitiveTopology primitiveTopology,
        std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount) = 0;

    virtual void DrawInstanced(PrimitiveTopology primitiveTopology, std::size_t vertexCount, std::size_t instanceCount) = 0;

    virtual void DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
        std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount, std::size_t instanceCount) = 0;

    ///@brief Retrieves the capabilities of a GraphicsContext
    virtual GraphicsCapabilities GetCapabilities() const = 0;

    virtual void SetViewport(Viewport const& viewport) = 0;

    virtual Rectangle GetScissorRectangle() const = 0;

    virtual void SetScissorRectangle(Rectangle const& rectangle) = 0;

    virtual void SetBlendFactor(Color const& blendFactor) = 0;

    virtual void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers) = 0;

    virtual void SetTexture(int index) = 0;

    virtual void SetTexture(int index, Texture2D & texture) = 0;

    virtual void SetTexture(int index, RenderTarget2D & texture) = 0;

    virtual void SetRenderTarget() = 0;

    virtual void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets) = 0;

    ///@details low-level API
    virtual void SetEffectPass(std::shared_ptr<NativeEffectPass> const& nativeEffectPass) = 0;

    ///@details low-level API
    virtual void SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& nativeConstantLayout) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_NATIVEGRAPHICSCONTEXT_D292A72E_HPP
