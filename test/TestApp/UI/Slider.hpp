//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SLIDER_8DA97A75_03EF_4C73_9B53_62B6BD9FB7BB_HPP
#define POMDOG_SLIDER_8DA97A75_03EF_4C73_9B53_62B6BD9FB7BB_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "Control.hpp"
#include "PointerPoint.hpp"

namespace Pomdog {
namespace UI {

class Slider: public Control {
public:
	Transform2D RenderTransform;
	Vector2 RenderTransformOrigin;

private:
	double minimum;
	double maximum;
	double value;
	
	std::uint16_t height;
	std::uint16_t width;

	struct ColorAnimation {
		Color startColor;
		Color targetColor;
		float duration = 1;
		float time = 0;
	};

	Optional<ColorAnimation> colorAnimation;

	Color borderColor;
	Color fillColor;
	Color trackColor;
	bool isDragging;
	bool isEnabled;
	
public:
	Slider(double minimum, double maximum);
	
	void Value(double valueIn);
	double Value() const;
	
	double Minimum() const;
	double Maximum() const;
	
	bool IsEnabled() const;
	void IsEnabled(bool isEnabled);
	
	void OnPointerCanceled(PointerPoint const& pointerPoint) override;
	
	void OnPointerCaptureLost(PointerPoint const& pointerPoint) override;
	
	void OnPointerWheelChanged(PointerPoint const& pointerPoint) override;
	
	void OnPointerEntered(PointerPoint const& pointerPoint) override;
	
	void OnPointerExited(PointerPoint const& pointerPoint) override;
	
	void OnPointerPressed(PointerPoint const& pointerPoint) override;
	
	void OnPointerMoved(PointerPoint const& pointerPoint) override;
	
	void OnPointerReleased(PointerPoint const& pointerPoint) override;
	
	void Draw(DrawingContext & drawingContext) override;
	
	void UpdateAnimation(DurationSeconds const& frameDuration) override;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_SLIDER_8DA97A75_03EF_4C73_9B53_62B6BD9FB7BB_HPP)
