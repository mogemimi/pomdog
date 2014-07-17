//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MAIDBEAMGAME_EC7DAEEE_4167_40CD_931D_B8627B24F68E_HPP
#define POMDOG_MAIDBEAMGAME_EC7DAEEE_4167_40CD_931D_B8627B24F68E_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "SceneEditor/InGameEditor.hpp"
#include "UI/DrawingContext.hpp"
#include "UI/UIElementHierarchy.hpp"
#include "UI/ScenePanel.hpp"
#include "UI/Slider.hpp"
#include "UI/TextBlock.hpp"
#include "UI/ToggleSwitch.hpp"
#include "Graphics/LineBatch.hpp"
#include "Rendering/Renderer.hpp"

namespace TestApp {

using namespace Pomdog;

class FXAA;

class MaidBeamGame: public Game {
public:
	explicit MaidBeamGame(std::shared_ptr<GameHost> const& gameHost);

	~MaidBeamGame();

	void Initialize();
	
	void Update();
	
	void Draw();

private:
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GraphicsContext> graphicsContext;

	std::unique_ptr<SceneEditor::InGameEditor> gameEditor;
	std::shared_ptr<UI::ScenePanel> scenePanel;
	std::shared_ptr<UI::Slider> slider1;
	std::shared_ptr<UI::Slider> slider2;
	std::shared_ptr<UI::ToggleSwitch> toggleSwitch1;
	std::shared_ptr<UI::ToggleSwitch> toggleSwitch2;
	std::shared_ptr<UI::ToggleSwitch> toggleSwitch3;
	std::shared_ptr<UI::ToggleSwitch> toggleSwitch4;
	std::shared_ptr<UI::TextBlock> textBlock1;

	GameWorld gameWorld;
	std::shared_ptr<GameObject> mainCamera;
	std::shared_ptr<GameObject> maid;
	std::shared_ptr<GameObject> lightningBeam;
	std::vector<std::shared_ptr<GameObject>> enemies;
	
	ScopedConnection clientSizeChangedConnection;
	
	std::unique_ptr<Renderer> renderer;
	std::shared_ptr<RenderTarget2D> renderTarget;
	std::unique_ptr<FXAA> fxaa;
};

}// namespace TestApp

#endif // !defined(POMDOG_MAIDBEAMGAME_EC7DAEEE_4167_40CD_931D_B8627B24F68E_HPP)
