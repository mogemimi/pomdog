// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_IMAGEEFFECTBASE_AE89AE0A_HPP
#define POMDOG_IMAGEEFFECTBASE_AE89AE0A_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <functional>

namespace Pomdog {

class ImageEffectBase {
public:
    virtual ~ImageEffectBase() = default;

    virtual void Apply(
        GraphicsCommandList & commandList,
        std::shared_ptr<RenderTarget2D> const& source,
        std::shared_ptr<ConstantBuffer> const& constantBuffer) = 0;

    virtual void PreRender(
        GraphicsCommandList & commandList,
        std::shared_ptr<ConstantBuffer> const& constantBuffer,
        std::function<void()> const& draw)
    {}
};

} // namespace Pomdog

#endif // POMDOG_IMAGEEFFECTBASE_AE89AE0A_HPP
