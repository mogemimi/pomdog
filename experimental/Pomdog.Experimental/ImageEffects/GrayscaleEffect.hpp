// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAYSCALEEFFECT_C2865F07_9F22_4BB6_8222_A317EE8C764A_HPP
#define POMDOG_GRAYSCALEEFFECT_C2865F07_9F22_4BB6_8222_A317EE8C764A_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class GrayscaleEffect {
public:
    explicit GrayscaleEffect(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    void SetViewport(float width, float height);
    void SetTexture(std::shared_ptr<RenderTarget2D> const& texture);

    void Apply(GraphicsContext & graphicsContext);

private:
    std::shared_ptr<RenderTarget2D> texture;
    std::shared_ptr<SamplerState> samplerLinear;
    std::shared_ptr<EffectPass> effectPass;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GRAYSCALEEFFECT_C2865F07_9F22_4BB6_8222_A317EE8C764A_HPP)
