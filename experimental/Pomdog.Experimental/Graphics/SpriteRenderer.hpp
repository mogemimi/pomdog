//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SPRITERENDERER_8ACAC405_03FF_42D3_87CA_D25B183EF883_HPP
#define POMDOG_SPRITERENDERER_8ACAC405_03FF_42D3_87CA_D25B183EF883_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "SpriteSortMode.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <memory>

namespace Pomdog {

class SpriteRenderer {
public:
	SpriteRenderer(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	~SpriteRenderer();

	void SetProjectionMatrix(Matrix4x4 const& projectionMatrix);

	void Begin(SpriteSortMode sortMode);

	void Begin(SpriteSortMode sortMode, Matrix4x4 const& transformMatrix);

	void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
		Rectangle const& sourceRect, Color const& color);

	void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
		Vector2 const& position, Color const& color);

	void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
		Vector2 const& position, Rectangle const& sourceRect, Color const& color);

	void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
		Vector2 const& position, Rectangle const& sourceRect, Color const& color,
		Radian<float> const& rotation, Vector2 const& originPivot, float scale, float layerDepth);

	void Draw(std::shared_ptr<Texture2D> const& texture, Matrix3x2 const& worldMatrix,
		Vector2 const& position, Rectangle const& sourceRect, Color const& color,
		Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth);

	void End();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SPRITERENDERER_8ACAC405_03FF_42D3_87CA_D25B183EF883_HPP)
