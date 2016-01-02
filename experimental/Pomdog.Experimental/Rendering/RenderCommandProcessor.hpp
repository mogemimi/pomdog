// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"

namespace Pomdog {

class RenderCommandProcessor {
public:
    virtual ~RenderCommandProcessor() = default;

    virtual void SetViewProjection(Matrix4x4 const& view, Matrix4x4 const& projection) = 0;

    virtual void Begin(GraphicsCommandQueue & commandQueue) = 0;

    virtual void Draw(GraphicsCommandQueue & commandQueue, RenderCommand & command) = 0;

    virtual void End(GraphicsCommandQueue & commandQueue) = 0;

    virtual int GetDrawCallCount() const noexcept = 0;
};

} // namespace Pomdog
