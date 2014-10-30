//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "PrimitiveCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitiveCommand.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
PrimitiveCommandProcessor::PrimitiveCommandProcessor(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
	: primitiveBatch(graphicsContext, graphicsDevice)
	, drawCallCount(0)
{
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::Begin(GraphicsContext & graphicsContext)
{
	drawCallCount = 0;
	
	primitiveBatch.Begin(viewProjection);
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::Draw(GraphicsContext & graphicsContext, RenderCommand & command)
{
	using Details::Rendering::PrimitiveCommand;

	auto & primitiveCommand = static_cast<PrimitiveCommand &>(command);
	primitiveBatch.DrawRectangle(primitiveCommand.transform,
		primitiveCommand.rectangle,
		primitiveCommand.leftBottomColor, primitiveCommand.rightBottomColor,
		primitiveCommand.rightTopColor, primitiveCommand.leftTopColor);
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::End(GraphicsContext & graphicsContext)
{
	primitiveBatch.End();
		
	///@todo Not implemented
	//drawCallCount += primitiveBatch.DrawCallCount();
}
//-----------------------------------------------------------------------
int PrimitiveCommandProcessor::DrawCallCount() const
{
	return drawCallCount;
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::SetViewProjection(Matrix4x4 const& view, Matrix4x4 const& projection)
{
	viewProjection = view * projection;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
