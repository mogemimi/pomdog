// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_NATIVEPIPELINESTATE_F0C96CAC_HPP
#define POMDOG_NATIVEPIPELINESTATE_F0C96CAC_HPP

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

#endif // POMDOG_NATIVEPIPELINESTATE_F0C96CAC_HPP
