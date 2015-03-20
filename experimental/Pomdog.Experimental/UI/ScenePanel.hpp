//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SCENEPANEL_F59B1210_FA6C_4446_9752_7754A8017116_HPP
#define POMDOG_SCENEPANEL_F59B1210_FA6C_4446_9752_7754A8017116_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Panel.hpp"
#include "detail/UIEventConnection.hpp"
#include "Pomdog.Experimental/Gameplay/GameObject.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace UI {

class ScenePanel: public Panel, public std::enable_shared_from_this<ScenePanel> {
public:
	GameObject cameraObject;

public:
	ScenePanel(std::uint32_t widthIn, std::uint32_t heightIn);

	bool IsEnabled() const;
	void IsEnabled(bool isEnabled);

	bool SizeToFitContent() const override { return true; }

	void OnParentChanged() override;

	void OnPointerWheelChanged(PointerPoint const& pointerPoint) override;

	void OnPointerEntered(PointerPoint const& pointerPoint) override;

	void OnPointerExited(PointerPoint const& pointerPoint) override;

	void OnPointerPressed(PointerPoint const& pointerPoint) override;

	void OnPointerMoved(PointerPoint const& pointerPoint) override;

	void OnPointerReleased(PointerPoint const& pointerPoint) override;

	void OnRenderSizeChanged(std::uint32_t width, std::uint32_t height) override;

	void Draw(DrawingContext & drawingContext) override;

	void UpdateAnimation(Duration const& frameDuration) override;

	// Events:
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
	Detail::UIEventConnection connection;

	Optional<Vector2> tumbleStartPosition;
	Optional<Vector2> trackStartPosition;

	Duration timer;
	float normalizedScrollDirection;
	float scrollAcceleration;

	bool isFocused;
	bool isEnabled;

	class ScrollWheelSampler {
	private:
		///@todo replace with std::deque<float>
		Optional<float> average;

	public:
		void AddWheelDelta(int wheelDelta)
		{
			if (wheelDelta == 0) {
				return;
			}

			if (!average) {
				average = std::abs(wheelDelta);
			}
			else {
				average = std::max((*average + std::abs(wheelDelta)) / 2, 1.0f);
			}
		}

		float GetScrollWheelDeltaAverage() const
		{
			if (average) {
				return *average;
			}
			return 1.0f;
		}
	};

	ScrollWheelSampler scrollWheelSampler;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_SCENEPANEL_F59B1210_FA6C_4446_9752_7754A8017116_HPP)
