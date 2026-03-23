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

struct RenderTargetBlendDescGL4 final {
    BlendGL4 colorSource;
    BlendGL4 colorDestination;
    BlendOperationGL4 colorOperation;
    BlendGL4 alphaSource;
    BlendGL4 alphaDestination;
    BlendOperationGL4 alphaOperation;
    bool blendEnable = false;
};

class BlendStateGL4 final {
private:
    std::array<RenderTargetBlendDescGL4, 8> renderTargets_;
    bool independentBlendEnable_ = false;
    bool alphaToCoverageEnable_ = false;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const BlendDesc& descriptor) noexcept;

    void apply();
};

} // namespace pomdog::gpu::detail::gl4
