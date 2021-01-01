// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT PipelineState {
public:
    PipelineState() noexcept;
    PipelineState(const PipelineState&) = delete;
    PipelineState& operator=(const PipelineState&) = delete;

    virtual ~PipelineState();
};

} // namespace Pomdog
