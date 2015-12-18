// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {

class NativePipelineState {
public:
    NativePipelineState() = default;
    NativePipelineState(NativePipelineState const&) = delete;
    NativePipelineState & operator=(NativePipelineState const&) = delete;

    virtual ~NativePipelineState() = default;
};

} // namespace Detail
} // namespace Pomdog
