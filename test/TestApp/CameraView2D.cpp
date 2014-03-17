//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CameraView2D.hpp"

namespace TestApp {
namespace {

static Vector2 SampleTrackGesture(MouseState const& mouseState, std::unique_ptr<Vector2> & gesture)
{
	if (gesture) {
		Vector2 position {
			static_cast<float>(mouseState.Position.X),
			static_cast<float>(mouseState.Position.Y)
		};

		constexpr float threshold = 2.0f;

		Vector2 delta = Vector2::Zero;
		
		if (threshold < Vector2::Distance(*gesture, position)) {
			delta = position - *gesture;
			*gesture = std::move(position);
		}

		if (mouseState.MiddleButton == ButtonState::Released) {
			gesture.reset();
		}
		
		return delta;
	}
	else {
		if (mouseState.MiddleButton == ButtonState::Pressed)
		{
			gesture = std::unique_ptr<Vector2>(new Vector2);
			gesture->X = mouseState.Position.X;
			gesture->Y = mouseState.Position.Y;
		}
	}
	
	return Vector2::Zero;
}
//-----------------------------------------------------------------------
static Radian<float> SampleTumbleGesture(MouseState const& mouseState, Rectangle const& viewportSize, std::unique_ptr<Vector2> & gesture)
{
	if (!gesture)
	{
		if (mouseState.LeftButton == ButtonState::Pressed)
		{
			gesture = std::unique_ptr<Vector2>(new Vector2);
			gesture->X = mouseState.Position.X;
			gesture->Y = mouseState.Position.Y;
		}
	}
	else
	{
		Vector2 position {
			static_cast<float>(mouseState.Position.X),
			static_cast<float>(mouseState.Position.Y)
		};

		constexpr float threshold = 1.0f;

		Radian<float> delta;
		
		auto const distance = Vector2::Distance(*gesture, position);

		if (threshold < distance)
		{
			auto const centerPoint = viewportSize.Center();
			Vector2 const screenOffset(centerPoint.X, centerPoint.Y);

			auto vecInScreen1 = (*gesture - screenOffset);
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
				
				*gesture = std::move(position);
			}
		}

		if (mouseState.LeftButton == ButtonState::Released) {
			gesture.reset();
		}
		
		return delta;
	}
	
	return 0.0f;
}

}// unnamed namespace
//-----------------------------------------------------------------------
CameraView2D::CameraView2D()
	: prevScrollWheel(0)
	, scrollWheel(0.0f)
{
}
//-----------------------------------------------------------------------
void CameraView2D::Input(MouseState const& mouseState, Rectangle const& viewportSize, Transform2D & transform, Camera2D & camera)
{
	{
		scrollWheel += ((mouseState.ScrollWheel - prevScrollWheel) * 0.02f);
	
		camera.Zoom(MathHelper::Clamp(camera.Zoom() + (camera.Zoom() * scrollWheel), 0.05f, 100.0f));
		
		scrollWheel = scrollWheel * 0.8f;// TODO: Replace with deltaTime
		prevScrollWheel = mouseState.ScrollWheel;
	}
	{
		// Tumble Gesture
		transform.Rotation += SampleTumbleGesture(mouseState, viewportSize, tumbleStartPosition);
		if (transform.Rotation > MathConstants<float>::TwoPi()) {
			transform.Rotation -= MathConstants<float>::TwoPi();
		}
		else if (transform.Rotation < -MathConstants<float>::TwoPi()) {
			transform.Rotation += MathConstants<float>::TwoPi();
		}
		POMDOG_ASSERT(transform.Rotation != std::numeric_limits<float>::quiet_NaN());
		POMDOG_ASSERT(transform.Rotation != std::numeric_limits<float>::infinity());
	}
	{
		// Track Gesture
		auto delta = SampleTrackGesture(mouseState, trackStartPosition);
		auto matrix = (Matrix3x3::CreateTranslation(delta) *
		Matrix3x3::CreateScale(1.0f/camera.Zoom()) *
		Matrix3x3::CreateRotationZ(transform.Rotation));
		
		transform.Position -= {matrix(2, 0), matrix(2, 1)};
	}
	
}

}// namespace TestApp
