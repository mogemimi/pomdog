// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Utility/Errors.hpp"

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
