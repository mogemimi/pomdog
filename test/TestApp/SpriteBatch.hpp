//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITEBATCH_B4DEBA3B_98DD_4767_A79A_4CAFC8A81AF4_HPP
#define POMDOG_SPRITEBATCH_B4DEBA3B_98DD_4767_A79A_4CAFC8A81AF4_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

class SpriteBatch {
public:
	explicit SpriteBatch(std::shared_ptr<GameHost> const& gameHost);
	
	~SpriteBatch();
	
	void Begin(Matrix3x3 const& transformMatrix);
	
	void Draw(std::shared_ptr<Texture2D> const& texture,
		Vector2 const& position, Vector2 const& scale, Radian<float> const& rotation,
		Rectangle const& sourceRect, Vector2 const& originPivot, float layerDepth);
		
	void End();
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace TestApp

#endif // !defined(POMDOG_SPRITEBATCH_B4DEBA3B_98DD_4767_A79A_4CAFC8A81AF4_HPP)
