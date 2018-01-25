// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {

class NativePipelineState {
public:
    NativePipelineState() = default;
    NativePipelineState(const NativePipelineState&) = delete;
    NativePipelineState & operator=(const NativePipelineState&) = delete;

    virtual ~NativePipelineState() = default;
};

} // namespace Detail
} // namespace Pomdog
