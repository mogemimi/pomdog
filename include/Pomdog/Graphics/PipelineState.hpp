// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT PipelineState {
public:
    PipelineState() = delete;
    PipelineState(PipelineState const&) = delete;
    PipelineState(PipelineState &&) = default;

    PipelineState(GraphicsDevice & graphicsDevice,
        PipelineStateDescription const& description);

    PipelineState(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        PipelineStateDescription const& description);

    ~PipelineState();

    PipelineState & operator=(PipelineState const&) = delete;
    PipelineState & operator=(PipelineState &&) = default;

public:
    Detail::NativePipelineState* NativePipelineState();

private:
    std::unique_ptr<Detail::NativePipelineState> nativePipelineState;
};

} // namespace Pomdog
