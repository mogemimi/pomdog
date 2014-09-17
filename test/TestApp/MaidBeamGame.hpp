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

#include <Pomdog.Experimental/Pomdog2D.hpp>
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class SpriteRenderable;

}// namespace Pomdog

namespace TestApp {

using namespace Pomdog;

class MaidBeamGame: public Game {
public:
	explicit MaidBeamGame(std::shared_ptr<GameHost> const& gameHost);

	~MaidBeamGame();

	void Initialize();
	
	void Update();
	
	void Draw();

private:
	void DrawScene(Transform2D const& transform, Camera2D const& camera);

private:
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GraphicsContext> graphicsContext;

	std::unique_ptr<SceneEditor::InGameEditor> gameEditor;
	bool sandboxMode;
	
	std::shared_ptr<UI::ScenePanel> scenePanel;
	std::shared_ptr<UI::Slider> slider1;
	std::shared_ptr<UI::Slider> slider2;
	std::shared_ptr<UI::ToggleSwitch> toggleSwitch1;
	std::shared_ptr<UI::ToggleSwitch> toggleSwitch2;
	std::shared_ptr<UI::TextBlock> textBlock1;
	
	GameWorld gameWorld;
	std::unique_ptr<GameLevel> level;
	GameObject editorCamera;
	
	std::unique_ptr<Renderer> renderer;
	std::shared_ptr<RenderTarget2D> renderTarget;
	std::unique_ptr<FXAA> fxaa;
	std::unique_ptr<ScreenQuad> screenQuad;
	std::vector<SpriteRenderable> cameraSprites;
	
	ScopedConnection clientSizeChangedConnection;
	ScopedConnection cameraChangedConnection;
};

}// namespace TestApp

#endif // !defined(POMDOG_MAIDBEAMGAME_EC7DAEEE_4167_40CD_931D_B8627B24F68E_HPP)
