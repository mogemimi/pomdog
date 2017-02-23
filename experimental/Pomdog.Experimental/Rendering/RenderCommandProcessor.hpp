// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include <typeindex>

namespace Pomdog {

class RenderCommandProcessor {
public:
    virtual ~RenderCommandProcessor() = default;

    virtual void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const Matrix4x4& viewProjection) = 0;

    virtual void Draw(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        RenderCommand & command) = 0;

    virtual void FlushBatch() = 0;

    virtual void End() = 0;

    virtual int GetDrawCallCount() const noexcept = 0;

    virtual std::type_index GetCommandType() const noexcept = 0;
};

} // namespace Pomdog
