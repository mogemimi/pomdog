// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

using BlendGL4 = pomdog::detail::Tagged<GLenum, BlendFactor>;
using BlendOperationGL4 = pomdog::detail::Tagged<GLenum, BlendOperation>;

struct RenderTargetBlendDescriptorGL4 final {
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
    Initialize(const BlendDescriptor& descriptor) noexcept;

    void Apply();

private:
    std::array<RenderTargetBlendDescriptorGL4, 8> renderTargets;
    bool independentBlendEnable = false;
    bool alphaToCoverageEnable = false;
};

} // namespace pomdog::gpu::detail::gl4
