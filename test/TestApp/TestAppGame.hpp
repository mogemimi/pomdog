//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COCOATESTGAME_HPP
#define POMDOG_COCOATESTGAME_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "CameraView2D.hpp"

namespace TestApp {

using namespace Pomdog;

class PrimitiveAxes;
class PrimitiveGrid;
class SpriteRenderer;
class SpriteBatch;
class FXAA;

class TestAppGame: public Game {
public:
	explicit TestAppGame(std::shared_ptr<GameHost> const& gameHost);

	~TestAppGame();

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
	std::unique_ptr<SpriteRenderer> spriteRenderer;
	
	std::shared_ptr<SamplerState> samplerPoint;
	std::shared_ptr<RenderTarget2D> renderTarget;
	std::unique_ptr<FXAA> fxaa;
	
	CameraView2D cameraView;
	
	GameWorld gameWorld;
	GameObjectID mainCameraID;
	GameObjectID rootObjectID;
	//std::shared_ptr<Scene> hierarchy;
};

}// namespace TestApp

#endif // !defined(POMDOG_COCOATESTGAME_HPP)
