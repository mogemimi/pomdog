//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PRIMITIVECOMMANDPROCESSOR_78CC3F16_1297_4C1B_BE10_2F174451E3B4_HPP
#define POMDOG_PRIMITIVECOMMANDPROCESSOR_78CC3F16_1297_4C1B_BE10_2F174451E3B4_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog.Experimental/Graphics/PolygonBatch.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class PrimitiveCommandProcessor final: public RenderCommandProcessor {
public:
	PrimitiveCommandProcessor(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	void Begin(GraphicsContext & graphicsContext) override;

	void Draw(GraphicsContext & graphicsContext, RenderCommand & command) override;

	void End(GraphicsContext & graphicsContext) override;

	int DrawCallCount() const override;

	void SetViewProjection(Matrix4x4 const& view, Matrix4x4 const& projection) override;

public:
	Matrix4x4 viewProjection;
	PolygonBatch primitiveBatch;
	std::uint32_t drawCallCount;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PRIMITIVECOMMANDPROCESSOR_78CC3F16_1297_4C1B_BE10_2F174451E3B4_HPP)
