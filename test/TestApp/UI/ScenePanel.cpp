//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ScenePanel.hpp"
#include <Pomdog/Utility/MakeUnique.hpp>
#include "PointerPoint.hpp"

namespace Pomdog {
namespace UI {
namespace {
//-----------------------------------------------------------------------
static Radian<float> SampleTumbleGesture(Vector2 const& position, Vector2 & startPosition, Rectangle const& viewportSize)
{
	constexpr float threshold = 1.0f;

	Radian<float> delta = 0.0f;
	
	auto const distance = Vector2::Distance(startPosition, position);

	if (threshold < distance)
	{
		auto const centerPoint = viewportSize.Center();
		Vector2 const screenOffset(centerPoint.X, centerPoint.Y);

		auto vecInScreen1 = (startPosition - screenOffset);
		auto vecInScreen2 = (position - screenOffset);
		auto const cross = Vector2::Cross(vecInScreen2, vecInScreen1);

		if (std::abs(cross) > 1.0f)
		{
			auto const cosAngle = Vector2::Dot(vecInScreen1, vecInScreen2)/(vecInScreen1.Length() * vecInScreen2.Length());
			delta = std::acos(MathHelper::Clamp(cosAngle, -1.0f, 1.0f)) * (cross > 0.0f ? 1.0f: -1.0f);
			POMDOG_ASSERT(delta != std::numeric_limits<float>::signaling_NaN());
			
			if ((vecInScreen1.Length() < 24.0f || vecInScreen2.Length() < 24.0f)) {
				delta *= std::min(std::abs(cross/vecInScreen2.Length())*0.01f, 1.0f);
			}
			
			startPosition = std::move(position);
		}
	}
	
	return delta;
}
//-----------------------------------------------------------------------
static Vector2 SampleTrackGesture(Vector2 const& position, Vector2 & startPosition)
{
	constexpr float threshold = 2.0f;

	Vector2 delta = Vector2::Zero;
	
	if (threshold < Vector2::Distance(startPosition, position)) {
		delta = position - startPosition;
		startPosition = position;
	}
	
	return delta;
}

}// unnamed namespace
//-----------------------------------------------------------------------
ScenePanel::ScenePanel(std::uint32_t widthIn, std::uint32_t heightIn)
	: Panel(Matrix3x2::Identity, widthIn, heightIn)
	, isFocused(false)
	, prevScrollWheel(0)
	, scrollWheel(0)
{}
//-----------------------------------------------------------------------
Vector2 ScenePanel::ConvertToPanelSpace(Point2D const& point) const
{
	return Vector2(point.X, Height() - point.Y);
}
//-----------------------------------------------------------------------
void ScenePanel::OnPointerCanceled(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void ScenePanel::OnPointerCaptureLost(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void ScenePanel::OnPointerWheelChanged(PointerPoint const& pointerPoint)
{
	if (prevScrollWheel) {
		*prevScrollWheel += pointerPoint.MouseWheelDelta;
	}
	else {
		prevScrollWheel = pointerPoint.MouseWheelDelta;
	}
}
//-----------------------------------------------------------------------
void ScenePanel::OnPointerEntered(PointerPoint const& pointerPoint)
{
	isFocused = true;
}
//-----------------------------------------------------------------------
void ScenePanel::OnPointerExited(PointerPoint const& pointerPoint)
{
	isFocused = false;
}
//-----------------------------------------------------------------------
void ScenePanel::OnPointerPressed(PointerPoint const& pointerPoint)
{
	POMDOG_ASSERT(pointerPoint.MouseEvent);
	
	switch (*pointerPoint.MouseEvent) {
	case PointerMouseEvent::LeftButtonPressed:
		OnMouseLeftButtonPressed(pointerPoint);
		break;
	case PointerMouseEvent::MiddleButtonPressed:
		OnMouseMiddleButtonPressed(pointerPoint);
		break;
	case PointerMouseEvent::RightButtonPressed:
		OnMouseRightButtonPressed(pointerPoint);
		break;
	default:
		break;
	}
}
//-----------------------------------------------------------------------
void ScenePanel::OnPointerMoved(PointerPoint const& pointerPoint)
{
	POMDOG_ASSERT(pointerPoint.MouseEvent);
	
	switch (*pointerPoint.MouseEvent) {
	case PointerMouseEvent::LeftButtonPressed:
		OnMouseLeftButtonMoved(pointerPoint);
		break;
	case PointerMouseEvent::MiddleButtonPressed:
		OnMouseMiddleButtonMoved(pointerPoint);
		break;
	case PointerMouseEvent::RightButtonPressed:
		OnMouseRightButtonMoved(pointerPoint);
		break;
	default:
		break;
	}
}
//-----------------------------------------------------------------------
void ScenePanel::OnPointerReleased(PointerPoint const& pointerPoint)
{
	tumbleStartPosition = OptionalType::NullOptional;
	trackStartPosition = OptionalType::NullOptional;
}
//-----------------------------------------------------------------------
void ScenePanel::OnMouseLeftButtonPressed(PointerPoint const& pointerPoint)
{
	if (!tumbleStartPosition)
	{
		tumbleStartPosition = ConvertToPanelSpace(pointerPoint.Position);
	}
}
//-----------------------------------------------------------------------
void ScenePanel::OnMouseLeftButtonMoved(PointerPoint const& pointerPoint)
{
	if (!tumbleStartPosition) {
		return;
	}

	auto transform = cameraObject->Component<Transform2D>();
	auto camera = cameraObject->Component<Camera2D>();
	
	if (!transform || !camera) {
		return;
	}

	{
		// Tumble Gesture
		transform->Rotation += SampleTumbleGesture(ConvertToPanelSpace(pointerPoint.Position), *tumbleStartPosition,
			Rectangle{0, 0, Width(), Height()});
		if (transform->Rotation > MathConstants<float>::TwoPi()) {
			transform->Rotation -= MathConstants<float>::TwoPi();
		}
		else if (transform->Rotation < -MathConstants<float>::TwoPi()) {
			transform->Rotation += MathConstants<float>::TwoPi();
		}
		POMDOG_ASSERT(transform->Rotation != std::numeric_limits<float>::quiet_NaN());
		POMDOG_ASSERT(transform->Rotation != std::numeric_limits<float>::infinity());
	}
}
//-----------------------------------------------------------------------
void ScenePanel::OnMouseMiddleButtonPressed(PointerPoint const& pointerPoint)
{
	trackStartPosition = ConvertToPanelSpace(pointerPoint.Position);
}
//-----------------------------------------------------------------------
void ScenePanel::OnMouseMiddleButtonMoved(PointerPoint const& pointerPoint)
{
	if (!trackStartPosition) {
		return;
	}
	
	auto transform = cameraObject->Component<Transform2D>();
	auto camera = cameraObject->Component<Camera2D>();
	
	if (!transform || !camera) {
		return;
	}

	// Track Gesture
	auto delta = SampleTrackGesture(ConvertToPanelSpace(pointerPoint.Position), *trackStartPosition);
	auto matrix = (Matrix3x3::CreateTranslation(delta)
		* Matrix3x3::CreateScale(1.0f / camera->Zoom())
		* Matrix3x3::CreateRotationZ(transform->Rotation));
	
	transform->Position -= {matrix(2, 0), matrix(2, 1)};
}
//-----------------------------------------------------------------------
void ScenePanel::OnMouseRightButtonPressed(PointerPoint const& pointerPoint)
{
	SceneTouch(ConvertToPanelSpace(pointerPoint.Position));
}
//-----------------------------------------------------------------------
void ScenePanel::OnMouseRightButtonMoved(PointerPoint const& pointerPoint)
{
	SceneTouch(ConvertToPanelSpace(pointerPoint.Position));
}
//-----------------------------------------------------------------------
void ScenePanel::UpdateAnimation(DurationSeconds const& frameDuration)
{
	auto transform = cameraObject->Component<Transform2D>();
	auto camera = cameraObject->Component<Camera2D>();
	
	if (!transform || !camera) {
		return;
	}
	
	constexpr float minZoom = 0.05f;
	constexpr float maxZoom = 100.0f;
	
	auto duration = std::min(frameDuration.count(), 1.0);
	
	if (prevScrollWheel) {
		scrollWheel += ((*prevScrollWheel) * duration);
		prevScrollWheel = OptionalType::NullOptional;
	}
	
	camera->Zoom(MathHelper::Clamp(camera->Zoom() + (camera->Zoom() * scrollWheel), minZoom, maxZoom));
	scrollWheel = scrollWheel * 0.8f;
}
//-----------------------------------------------------------------------
void ScenePanel::OnRenderSizeChanged(std::uint32_t widthIn, std::uint32_t heightIn)
{
	Width(widthIn);
	Height(heightIn);
}
//-----------------------------------------------------------------------
void ScenePanel::Draw(DrawingContext &)
{
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
