//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLETESTGAME_CB0334A7_637D_4E14_942A_86D6617BFF5D_HPP
#define POMDOG_PARTICLETESTGAME_CB0334A7_637D_4E14_942A_86D6617BFF5D_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "CameraView2D.hpp"
#include "ParticleSystem.hpp"

namespace TestApp {

using namespace Pomdog;

class PrimitiveAxes;
class PrimitiveGrid;
class SpriteBatch;
class FXAA;

class ParticleTestGame: public Game {
public:
	explicit ParticleTestGame(std::shared_ptr<GameHost> const& gameHost);

	~ParticleTestGame();

	void Initialize();
	
	void Update();
	
	void Draw();
	
private:
	void DrawSprites();

private:
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GraphicsContext> graphicsContext;
		
	std::shared_ptr<Texture2D> texture;
	std::unique_ptr<PrimitiveAxes> primitiveAxes;
	std::unique_ptr<PrimitiveGrid> primitiveGrid;
	std::unique_ptr<SpriteBatch> spriteBatch;
	
	std::shared_ptr<SamplerState> samplerPoint;
	std::shared_ptr<RenderTarget2D> renderTarget;
	std::unique_ptr<FXAA> fxaa;
	
	std::shared_ptr<BlendState> blendStateAdditive;
	std::shared_ptr<BlendState> blendStateNonPremultiplied;
	
	CameraView2D cameraView;
	
	GameWorld gameWorld;
	GameObjectID mainCameraID;
	GameObjectID rootObjectID;
	
	ParticleSystem particleSystem;
};

}// namespace TestApp

#endif // !defined(POMDOG_PARTICLETESTGAME_CB0334A7_637D_4E14_942A_86D6617BFF5D_HPP)
