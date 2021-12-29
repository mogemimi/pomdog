// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/sampler_descriptor.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class POMDOG_EXPORT SamplerState {
public:
    SamplerState() noexcept;
    SamplerState(const SamplerState&) = delete;
    SamplerState& operator=(const SamplerState&) = delete;

    virtual ~SamplerState();
};

} // namespace pomdog::gpu
