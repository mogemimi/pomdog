// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT ImageEffectPreRenderable {
public:
    virtual ~ImageEffectPreRenderable();

    virtual void PreRender(
        GraphicsCommandList& commandList,
        const std::shared_ptr<ConstantBuffer>& constantBuffer,
        const std::function<void()>& draw) = 0;
};

class ImageEffectBase {
public:
    virtual ~ImageEffectBase();

    virtual void Apply(
        GraphicsCommandList& commandList,
        const std::shared_ptr<RenderTarget2D>& source,
        const std::shared_ptr<ConstantBuffer>& constantBuffer) = 0;
};

} // namespace Pomdog
