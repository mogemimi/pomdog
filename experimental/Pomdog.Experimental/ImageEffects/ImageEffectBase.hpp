// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_IMAGEEFFECTBASE_AE89AE0A_HPP
#define POMDOG_IMAGEEFFECTBASE_AE89AE0A_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class ImageEffectBase {
public:
    virtual ~ImageEffectBase() = default;

    virtual void BindConstantBuffer(
        std::shared_ptr<ConstantBuffer> const& constantBuffer) = 0;

    virtual void Apply(
        GraphicsCommandList & commandList,
        std::shared_ptr<RenderTarget2D> const& source) = 0;
};

} // namespace Pomdog

#endif // POMDOG_IMAGEEFFECTBASE_AE89AE0A_HPP
