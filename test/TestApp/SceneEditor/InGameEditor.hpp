//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_INGAMEEDITOR_9210CCAC_0DDA_4BF4_8FD8_467873B18326_HPP
#define POMDOG_INGAMEEDITOR_9210CCAC_0DDA_4BF4_8FD8_467873B18326_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Pomdog.hpp>
#include "EditorColorScheme.hpp"
#include "GradientPlane.hpp"
#include "PrimitiveAxes.hpp"
#include "PrimitiveGrid.hpp"
#include "../UI/UIElementHierarchy.hpp"

namespace Pomdog {

class SpriteBatch;
class SpriteFont;

}// namespace Pomdog

namespace Pomdog {
namespace SceneEditor {

class InGameEditor {
public:
	explicit InGameEditor(std::shared_ptr<GameHost> const& gameHost);

	void Update();

	void SetViewProjection(Matrix4x4 const& viewProjection);

	void BeginDraw(GraphicsContext & graphicsContext);

	void EndDraw(GraphicsContext & graphicsContext);
	
	void AddUIElement(std::shared_ptr<UI::UIElement> const& element);

private:
	void DrawGrids(GraphicsContext & graphicsContext);
	void DrawGUI();
	
private:
	std::shared_ptr<GameHost> gameHost;
	
	SceneEditor::EditorColorScheme editorColorScheme;

	std::unique_ptr<SceneEditor::GradientPlane> backgroundPlane;

	std::unique_ptr<SceneEditor::PrimitiveAxes> primitiveAxes;
	std::unique_ptr<SceneEditor::PrimitiveGrid> primitiveGrid;
	
	std::shared_ptr<DepthStencilState> depthStencilState;
	std::unique_ptr<SpriteBatch> spriteBatch;
	std::shared_ptr<SpriteFont> spriteFont;
	std::unique_ptr<SpriteBatch> spriteBatchDistanceField;
	std::shared_ptr<EffectPass> distanceFieldEffect;
	
	std::shared_ptr<Texture2D> pomdogTexture;
	
	Matrix4x4 viewProjectionMatrix;
	
	UI::UIElementHierarchy hierarchy;
	
	ScopedConnection clientSizeChangedConnection;
};

}// namespace SceneEditor
}// namespace Pomdog

#endif // !defined(POMDOG_INGAMEEDITOR_9210CCAC_0DDA_4BF4_8FD8_467873B18326_HPP)
