// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <array>

namespace Pomdog::Detail::GL4 {

using BlendGL4 = Tagged<GLenum, Blend>;
using BlendOperationGL4 = Tagged<GLenum, BlendOperation>;

struct RenderTargetBlendDescGL4 final {
    BlendGL4 ColorSource;
    BlendGL4 ColorDestination;
    BlendOperationGL4 ColorOperation;
    BlendGL4 AlphaSource;
    BlendGL4 AlphaDestination;
    BlendOperationGL4 AlphaOperation;
    bool BlendEnable = false;
};

class BlendStateGL4 final {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const BlendDescription& description) noexcept;

    void Apply();

private:
    std::array<RenderTargetBlendDescGL4, 8> renderTargets;
    bool independentBlendEnable = false;
    bool alphaToCoverageEnable = false;
};

} // namespace Pomdog::Detail::GL4
