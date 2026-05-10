// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
struct BlendDesc;
enum class BlendFactor : u8;
enum class BlendOperation : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {

using BlendGL4 = pomdog::detail::Tagged<GLenum, BlendFactor>;
using BlendOperationGL4 = pomdog::detail::Tagged<GLenum, BlendOperation>;

/// RenderTargetBlendDescGL4 holds the OpenGL-typed blend factors and operations for a single render target.
struct RenderTargetBlendDescGL4 final {
    BlendGL4 colorSource;
    BlendGL4 colorDestination;
    BlendOperationGL4 colorOperation;
    BlendGL4 alphaSource;
    BlendGL4 alphaDestination;
    BlendOperationGL4 alphaOperation;
    bool blendEnable = false;
};

/// BlendStateGL4 encapsulates OpenGL blend state for up to 8 independent render targets.
class BlendStateGL4 final {
private:
    std::array<RenderTargetBlendDescGL4, 8> renderTargets_;
    bool independentBlendEnable_ = false;
    bool alphaToCoverageEnable_ = false;

public:
    /// Converts the BlendDesc to GL-typed blend state.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const BlendDesc& descriptor) noexcept;

    /// Applies the blend state to the current OpenGL context.
    void apply();
};

} // namespace pomdog::gpu::detail::gl4
