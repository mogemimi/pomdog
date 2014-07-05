//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_LIGHTNINGTESTGAME_FCB927D5_9F3B_4B26_B625_A91D2BC743E8_HPP
#define POMDOG_LIGHTNINGTESTGAME_FCB927D5_9F3B_4B26_B625_A91D2BC743E8_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "SceneEditor/InGameEditor.hpp"
#include "UI/DrawingContext.hpp"
#include "UI/ScenePanel.hpp"
#include "UI/Slider.hpp"
#include "ParticleSystem.hpp"
#include "BeamSystem.hpp"


namespace Pomdog {

class SpriteBatch;
class SpriteFont;

}// namespace Pomdog

namespace TestApp {

using namespace Pomdog;

class FXAA;
class SpriteRenderer;

class LightningTestGame: public Game {
public:
	explicit LightningTestGame(std::shared_ptr<GameHost> const& gameHost);

	~LightningTestGame();

	void Initialize();
	
	void Update();
	
	void Draw();
	
private:
	void DrawSprites();

private:
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GraphicsContext> graphicsContext;
	
	std::shared_ptr<Texture2D> texture;
	std::unique_ptr<SpriteBatch> spriteBatch;
	std::unique_ptr<SpriteRenderer> spriteRenderer;
	
	//std::shared_ptr<SamplerState> samplerPoint;
	std::shared_ptr<RenderTarget2D> renderTarget;
	std::unique_ptr<FXAA> fxaa;
	
	std::shared_ptr<BlendState> blendStateAdditive;
	std::shared_ptr<BlendState> blendStateNonPremultiplied;
	
	std::unique_ptr<SceneEditor::InGameEditor> gameEditor;
	std::shared_ptr<UI::ScenePanel> scenePanel;
	std::shared_ptr<UI::Slider> slider1;
	std::shared_ptr<UI::Slider> slider2;
	std::shared_ptr<UI::Slider> slider3;
	std::shared_ptr<UI::Slider> slider4;
	std::shared_ptr<UI::Slider> slider5;

	GameWorld gameWorld;
	std::shared_ptr<HierarchyNode> rootNode;
	std::shared_ptr<GameObject> mainCamera;

	BeamSystem beamSystem;
	Vector2 touchPoint;

	ScopedConnection clientSizeChangedConnection;
};

}// namespace TestApp

#endif // !defined(POMDOG_LIGHTNINGTESTGAME_FCB927D5_9F3B_4B26_B625_A91D2BC743E8_HPP)
