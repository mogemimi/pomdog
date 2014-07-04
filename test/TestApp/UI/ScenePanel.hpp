//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SCENEPANEL_F59B1210_FA6C_4446_9752_7754A8017116_HPP
#define POMDOG_SCENEPANEL_F59B1210_FA6C_4446_9752_7754A8017116_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Utility/Optional.hpp>
#include "Panel.hpp"
#include "detail/UIEventConnection.hpp"

namespace Pomdog {
namespace UI {

class ScenePanel: public Panel, public std::enable_shared_from_this<ScenePanel> {
public:
	std::shared_ptr<GameObject> cameraObject;
	
public:
	ScenePanel(std::uint32_t widthIn, std::uint32_t heightIn);

	bool SizeToFitContent() const override { return true; }
	
	void OnParentChanged() override;
	
	void OnPointerCanceled(PointerPoint const& pointerPoint) override;
	
	void OnPointerCaptureLost(PointerPoint const& pointerPoint) override;
	
	void OnPointerWheelChanged(PointerPoint const& pointerPoint) override;
	
	void OnPointerEntered(PointerPoint const& pointerPoint) override;
	
	void OnPointerExited(PointerPoint const& pointerPoint) override;
	
	void OnPointerPressed(PointerPoint const& pointerPoint) override;
	
	void OnPointerMoved(PointerPoint const& pointerPoint) override;
	
	void OnPointerReleased(PointerPoint const& pointerPoint) override;
	
	void OnRenderSizeChanged(std::uint32_t width, std::uint32_t height) override;
	
	void Draw(DrawingContext & drawingContext) override;
	
	void UpdateAnimation(DurationSeconds const& frameDuration) override;
	
	Signal<void(Vector2 const& point)> SceneTouch;
	
private:
	void OnMouseLeftButtonPressed(PointerPoint const& pointerPoint);
	void OnMouseLeftButtonMoved(PointerPoint const& pointerPoint);
	void OnMouseMiddleButtonPressed(PointerPoint const& pointerPoint);
	void OnMouseMiddleButtonMoved(PointerPoint const& pointerPoint);
	void OnMouseRightButtonPressed(PointerPoint const& pointerPoint);
	void OnMouseRightButtonMoved(PointerPoint const& pointerPoint);

	Vector2 ConvertToPanelSpace(Point2D const& point) const;
	
private:
	Details::UIEventConnection connection;

	Optional<Vector2> tumbleStartPosition;
	Optional<Vector2> trackStartPosition;
	
	Optional<std::int32_t> prevScrollWheel;
	float scrollWheel;

	bool isFocused;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_SCENEPANEL_F59B1210_FA6C_4446_9752_7754A8017116_HPP)
