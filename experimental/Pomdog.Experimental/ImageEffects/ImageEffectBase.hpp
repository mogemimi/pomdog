// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <functional>

namespace Pomdog {

class ImageEffectPreRenderable {
public:
    virtual ~ImageEffectPreRenderable() = default;

    virtual void PreRender(
        GraphicsCommandList & commandList,
        const std::shared_ptr<ConstantBuffer>& constantBuffer,
        const std::function<void()>& draw) = 0;
};

class ImageEffectBase {
public:
    virtual ~ImageEffectBase() = default;

    virtual void Apply(
        GraphicsCommandList & commandList,
        const std::shared_ptr<RenderTarget2D>& source,
        const std::shared_ptr<ConstantBuffer>& constantBuffer) = 0;
};

} // namespace Pomdog
