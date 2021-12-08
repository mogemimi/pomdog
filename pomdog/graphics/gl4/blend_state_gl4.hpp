// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"
#include "pomdog/utility/errors.hpp"
#include "pomdog/utility/tagged.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::gl4 {

using BlendGL4 = Tagged<GLenum, BlendFactor>;
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

} // namespace pomdog::detail::gl4
