//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ParticleBatchCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Details {
namespace Rendering {
//-----------------------------------------------------------------------
void ParticleBatchCommand::Execute(GraphicsContext & graphicsContext)
{
//	auto blendStateOld = graphicsContext.GetBlendState();
//	graphicsContext.SetBlendState(blendState);
//
//	POMDOG_ASSERT(spriteRenderer);
//	spriteRenderer->Begin(SpriteSortMode::Deferred, modelViewProjection);
//	{
//		for (auto & particle: *particles)
//		{
//			spriteRenderer->Draw(texture, Matrix3x2::Identity, particle.Position,
//				Rectangle(0, 0, texture->Width(), texture->Height()),
//				particle.Color, particle.Rotation, {0.5f, 0.5f}, particle.Size, 0);
//		}
//	}
//	spriteRenderer->End();
//	graphicsContext.SetBlendState(blendStateOld);
}
//-----------------------------------------------------------------------
std::type_index ParticleBatchCommand::TypeIndex() const
{
	static const std::type_index index = typeid(ParticleBatchCommand);
	return index;
}
//-----------------------------------------------------------------------
}// namespace Rendering
}// namespace Details
}// namespace Pomdog
