//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Camera.hpp"
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Quaternion.hpp>
#include <Pomdog/Math/MathHelper.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
Camera::Camera()
	: viewportSize(800.0f, 480.0f)
	, nearClipDistance(0.01f)
	, farClipDistance(1000.0f)
	, fieldOfView(MathConstants<float>::PiOver4())
	, projectionType(ProjectionType::Perspective)
{}
//-----------------------------------------------------------------------
ProjectionType Camera::ProjectionType() const
{
	return projectionType;
}
//-----------------------------------------------------------------------
Vector2 Camera::ViewportSize() const
{
	return viewportSize;
}
//-----------------------------------------------------------------------
void Camera::ViewportSize(Vector2 const& viewportSizeIn)
{
	POMDOG_ASSERT_MESSAGE(0.0f < viewportSizeIn.x, "out of range");
	POMDOG_ASSERT_MESSAGE(0.0f < viewportSizeIn.y, "out of range");

	this->viewportSize = viewportSizeIn;
}
//-----------------------------------------------------------------------
float Camera::NearClipDistance() const
{
	return nearClipDistance;
}
//-----------------------------------------------------------------------
float Camera::FarClipDistance() const
{
	return farClipDistance;
}
//-----------------------------------------------------------------------
Radian<float> Camera::FieldOfView() const
{
	return fieldOfView;
}
//-----------------------------------------------------------------------
void Camera::FieldOfView(Radian<float> const& fieldOfViewIn)
{
	this->fieldOfView = fieldOfViewIn;
}
//-----------------------------------------------------------------------
void Camera::FieldOfView(Radian<float> && fieldOfViewIn)
{
	this->fieldOfView = std::move(fieldOfViewIn);
}
//-----------------------------------------------------------------------
float Camera::Aspect() const
{
	POMDOG_ASSERT(viewportSize.y != 0.0f);
	return viewportSize.x/viewportSize.y;
}
//-----------------------------------------------------------------------
void Camera::Perspective(Radian<float> const& fieldOfViewIn)
{
	projectionType = ProjectionType::Perspective;
	
	POMDOG_ASSERT(fieldOfView > 0.0f);
	this->fieldOfView = fieldOfViewIn;
}
//-----------------------------------------------------------------------
void Camera::Perspective(Radian<float> && fieldOfViewIn)
{
	projectionType = ProjectionType::Perspective;
	
	POMDOG_ASSERT(fieldOfView > 0.0f);
	this->fieldOfView = std::move(fieldOfViewIn);
}
//-----------------------------------------------------------------------
void Camera::Orthographic()
{
	projectionType = ProjectionType::Orthographic;
}
//-----------------------------------------------------------------------
Matrix4x4 Camera::CreateProjectionMatrix() const
{
	POMDOG_ASSERT_MESSAGE(Aspect() > 0.0f, "AspectRatio: out of range");
	POMDOG_ASSERT_MESSAGE(nearClipDistance < farClipDistance, "FarClipDistance: out of range");

	switch (projectionType) {
	case ProjectionType::Orthographic:
		return Matrix4x4::CreateOrthographicLH(viewportSize.x, viewportSize.y,
			nearClipDistance, farClipDistance);
		break;
	case ProjectionType::Perspective:
		break;
	}

	return Matrix4x4::CreatePerspectiveFieldOfViewLH(
		fieldOfView, this->Aspect(), nearClipDistance, farClipDistance);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
