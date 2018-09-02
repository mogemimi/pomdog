// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT PipelineState final {
public:
    PipelineState() = delete;
    PipelineState(const PipelineState&) = delete;
    PipelineState(PipelineState&&) = default;

    PipelineState(
        GraphicsDevice& graphicsDevice,
        const PipelineStateDescription& description);

    PipelineState(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const PipelineStateDescription& description);

    ~PipelineState();

    PipelineState& operator=(const PipelineState&) = delete;
    PipelineState& operator=(PipelineState&&) = default;

    Detail::NativePipelineState* GetNativePipelineState();

private:
    std::unique_ptr<Detail::NativePipelineState> nativePipelineState;
};

} // namespace Pomdog
