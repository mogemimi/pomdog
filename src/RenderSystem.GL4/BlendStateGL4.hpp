// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Color.hpp"
#include <array>

namespace Pomdog {
namespace Detail {
namespace GL4 {

using BlendGL4 = Tagged<GLenum, Blend>;
using BlendOperationGL4 = Tagged<GLenum, BlendOperation>;

struct RenderTargetBlendDescGL4 final {
    BlendGL4 ColorSource;
    BlendGL4 ColorDestination;
    BlendOperationGL4 ColorOperation;
    BlendGL4 AlphaSource;
    BlendGL4 AlphaDestination;
    BlendOperationGL4 AlphaOperation;
    bool BlendEnable;
};

class BlendStateGL4 final {
public:
    BlendStateGL4() = delete;

    explicit BlendStateGL4(const BlendDescription& description);

    void Apply();

private:
    std::array<RenderTargetBlendDescGL4, 8> renderTargets;
    bool independentBlendEnable;
    bool alphaToCoverageEnable;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
