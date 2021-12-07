// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"
#include "pomdog/graphics/sampler_state.hpp"
#include "pomdog/utility/errors.hpp"
#include "pomdog/utility/tagged.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::GL4 {

using SamplerObjectGL4 = Tagged<GLuint, SamplerState>;

class SamplerStateGL4 final : public SamplerState {
public:
    ~SamplerStateGL4() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const SamplerDescription& description) noexcept;

    void Apply(int index);

private:
    std::optional<SamplerObjectGL4> samplerObject;
};

} // namespace Pomdog::Detail::GL4
