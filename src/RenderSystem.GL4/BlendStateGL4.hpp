// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Math/Color.hpp"
#include <array>

namespace Pomdog {
namespace Detail {
namespace GL4 {

using BlendGL4 = Tagged<GLenum, Blend>;
using BlendFunctionGL4 = Tagged<GLenum, BlendFunction>;

struct RenderTargetBlendDescGL4 final {
    BlendGL4 ColorSource;
    BlendGL4 ColorDestination;
    BlendFunctionGL4 ColorFunction;
    BlendGL4 AlphaSource;
    BlendGL4 AlphaDestination;
    BlendFunctionGL4 AlphaFunction;
    bool BlendEnable;
};

class BlendStateGL4 final {
public:
    BlendStateGL4() = delete;

    explicit BlendStateGL4(BlendDescription const& description);

    void Apply();

private:
    std::array<RenderTargetBlendDescGL4, 8> renderTargets;
    bool independentBlendEnable;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
