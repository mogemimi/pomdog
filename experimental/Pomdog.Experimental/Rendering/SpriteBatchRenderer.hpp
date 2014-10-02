//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITEBATCHRENDERER_FAFB0E53_90D1_47DF_B4EA_C9D551D0721E_HPP
#define POMDOG_SPRITEBATCHRENDERER_FAFB0E53_90D1_47DF_B4EA_C9D551D0721E_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <memory>

namespace Pomdog {
namespace Details {
namespace Rendering {

class SpriteBatchRenderer {
public:
	SpriteBatchRenderer(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
	~SpriteBatchRenderer();

	void SetProjectionMatrix(Matrix4x4 const& projectionMatrix);

	void Begin(Matrix4x4 const& transformMatrix);
	
	void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
		Color const& color, Vector2 const& originPivot);
	
	void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
		Rectangle const& sourceRect, Color const& color, Vector2 const& originPivot);
	
	void End();
	
	std::uint32_t DrawCallCount() const;
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Rendering
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SPRITEBATCHRENDERER_FAFB0E53_90D1_47DF_B4EA_C9D551D0721E_HPP)
