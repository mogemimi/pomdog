// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RENDERCOMMANDPROCESSOR_98635350_997C_43FC_B4CE_ABDCE7FBC94E_HPP
#define POMDOG_RENDERCOMMANDPROCESSOR_98635350_997C_43FC_B4CE_ABDCE7FBC94E_HPP

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"

namespace Pomdog {

class RenderCommandProcessor {
public:
    virtual ~RenderCommandProcessor() = default;

    virtual void SetViewProjection(Matrix4x4 const& view, Matrix4x4 const& projection) = 0;

    virtual void Begin(GraphicsContext & graphicsContext) = 0;

    virtual void Draw(GraphicsContext & graphicsContext, RenderCommand & command) = 0;

    virtual void End(GraphicsContext & graphicsContext) = 0;

    virtual int DrawCallCount() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERCOMMANDPROCESSOR_98635350_997C_43FC_B4CE_ABDCE7FBC94E_HPP)
