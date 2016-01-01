// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT PipelineState final {
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

    Detail::NativePipelineState* GetNativePipelineState();

private:
    std::unique_ptr<Detail::NativePipelineState> nativePipelineState;
};

} // namespace Pomdog
