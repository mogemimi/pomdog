// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include <typeindex>

namespace Pomdog {

class RenderCommandProcessor {
public:
    virtual ~RenderCommandProcessor() = default;

    virtual void SetViewProjection(
        const Matrix4x4& view,
        const Matrix4x4& projection) = 0;

    virtual void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandList) = 0;

    virtual void Draw(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        RenderCommand & command) = 0;

    virtual void End(
        const std::shared_ptr<GraphicsCommandList>& commandList) = 0;

    virtual int GetDrawCallCount() const noexcept = 0;

    virtual std::type_index GetCommandType() const noexcept = 0;
};

} // namespace Pomdog
