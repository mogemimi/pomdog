// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BLENDSTATEGL4_5A1F407F_HPP
#define POMDOG_BLENDSTATEGL4_5A1F407F_HPP

#include "OpenGLPrerequisites.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Math/Color.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

using BlendGL4 = Tagged<GLenum, Blend>;
using BlendFunctionGL4 = Tagged<GLenum, BlendFunction>;

class BlendStateGL4 final {
public:
    BlendStateGL4() = delete;

    explicit BlendStateGL4(BlendDescription const& description);

    void Apply();

private:
    BlendFunctionGL4 alphaFunction;
    BlendGL4 alphaSource;
    BlendGL4 alphaDestination;

    BlendFunctionGL4 colorFunction;
    BlendGL4 colorSource;
    BlendGL4 colorDestination;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_BLENDSTATEGL4_5A1F407F_HPP
