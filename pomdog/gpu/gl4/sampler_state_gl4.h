// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
struct SamplerDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {

/// SamplerObjectGL4 is a type-safe wrapper around a GL sampler object name.
using SamplerObjectGL4 = pomdog::detail::Tagged<GLuint, SamplerState>;

/// SamplerStateGL4 is the OpenGL 4 implementation of SamplerState.
class SamplerStateGL4 final : public SamplerState {
private:
    std::optional<SamplerObjectGL4> samplerObject_;

public:
    ~SamplerStateGL4() override;

    /// Creates a GL sampler object from the descriptor.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const SamplerDesc& descriptor) noexcept;

    /// Binds the sampler object to the given texture unit index.
    void apply(int index);
};

} // namespace pomdog::gpu::detail::gl4
