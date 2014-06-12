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
#include "SceneEditor/EditorColorScheme.hpp"
#include "SceneEditor/GradientPlane.hpp"
#include "SceneEditor/PrimitiveAxes.hpp"
#include "SceneEditor/PrimitiveGrid.hpp"
#include "UI/DrawingContext.hpp"
#include "UI/UIElementHierarchy.hpp"
#include "UI/ScenePanel.hpp"
#include "UI/Slider.hpp"
#include "ParticleSystem.hpp"

namespace TestApp {

using namespace Pomdog;

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
	void DrawGUI();

private:
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GraphicsContext> graphicsContext;
		
	std::shared_ptr<Texture2D> texture;
	std::unique_ptr<SpriteBatch> spriteBatch;
	
	//std::shared_ptr<SamplerState> samplerPoint;
	std::shared_ptr<RenderTarget2D> renderTarget;
	std::unique_ptr<FXAA> fxaa;
	
	std::shared_ptr<BlendState> blendStateAdditive;
	std::shared_ptr<BlendState> blendStateNonPremultiplied;
	
	SceneEditor::EditorColorScheme editorColorScheme;
	std::unique_ptr<SceneEditor::GradientPlane> backgroundPlane;
	std::unique_ptr<SceneEditor::PrimitiveAxes> primitiveAxes;
	std::unique_ptr<SceneEditor::PrimitiveGrid> primitiveGrid;
	UI::UIElementHierarchy hierarchy;
	std::shared_ptr<UI::ScenePanel> scenePanel;
	std::shared_ptr<UI::Slider> slider1;
	std::shared_ptr<UI::Slider> slider2;
	
	GameWorld gameWorld;
	std::shared_ptr<HierarchyNode> rootNode;
	std::shared_ptr<GameObject> mainCamera;
	
	ParticleSystem particleSystem;
};

}// namespace TestApp

#endif // !defined(POMDOG_PARTICLETESTGAME_CB0334A7_637D_4E14_942A_86D6617BFF5D_HPP)
