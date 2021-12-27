// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/graphics/forward_declarations.h"
#include "pomdog/graphics/sampler_description.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT SamplerState {
public:
    SamplerState() noexcept;
    SamplerState(const SamplerState&) = delete;
    SamplerState& operator=(const SamplerState&) = delete;

    virtual ~SamplerState();
};

} // namespace pomdog
