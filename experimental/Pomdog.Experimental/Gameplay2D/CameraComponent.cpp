// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay2D/CameraComponent.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {

CameraComponent::CameraComponent() noexcept
    : zoom(1.0f)
    , viewport(0, 0, 800, 480)
    , fieldOfView(MathHelper::ToRadians<float>(45.0f))
    , normalizedViewportX(0.0f)
    , normalizedViewportY(0.0f)
    , normalizedViewportWidth(1.0f)
    , normalizedViewportHeight(1.0f)
    , distanceToNearPlane(0.001f)
    , distanceToFarPlane(1000.0f)
    , backgroundColor(Color::CornflowerBlue)
    , projectionType(ProjectionType::Perspective)
{
}

float CameraComponent::GetZoom() const noexcept
{
    return zoom;
}

void CameraComponent::SetZoom(float zoomIn) noexcept
{
     zoom = zoomIn;
}

float CameraComponent::GetNear() const noexcept
{
    return distanceToNearPlane;
}

void CameraComponent::SetNear(float nearIn) noexcept
{
    distanceToNearPlane = nearIn;
}

float CameraComponent::GetFar() const noexcept
{
    return distanceToFarPlane;
}

void CameraComponent::SetFar(float farIn) noexcept
{
    distanceToFarPlane = farIn;
}

Radian<float> CameraComponent::GetFieldOfView() const noexcept
{
    return fieldOfView;
}

void CameraComponent::SetFieldOfView(const Radian<float>& fieldOfViewIn) noexcept
{
    POMDOG_ASSERT(fieldOfViewIn.value > 0);
    fieldOfView = fieldOfViewIn;
}

float CameraComponent::GetNormalizedViewportX() const noexcept
{
    return normalizedViewportX;
}

void CameraComponent::SetNormalizedViewportX(float viewportXIn) noexcept
{
    normalizedViewportX = viewportXIn;
}

float CameraComponent::GetNormalizedViewportY() const noexcept
{
    return normalizedViewportY;
}

void CameraComponent::SetNormalizedViewportY(float viewportYIn) noexcept
{
    normalizedViewportY = viewportYIn;
}

float CameraComponent::GetNormalizedViewportWidth() const noexcept
{
    return normalizedViewportWidth;
}

void CameraComponent::SetNormalizedViewportWidth(float viewportWidthIn) noexcept
{
    normalizedViewportWidth = viewportWidthIn;
}

float CameraComponent::GetNormalizedViewportHeight() const noexcept
{
    return normalizedViewportHeight;
}

void CameraComponent::SetNormalizedViewportHeight(float viewportHeightIn) noexcept
{
    normalizedViewportHeight = viewportHeightIn;
}

Color CameraComponent::GetBackgroundColor() const noexcept
{
    return backgroundColor;
}

void CameraComponent::SetBackgroundColor(const Color& color) noexcept
{
    backgroundColor = color;
}

ProjectionType CameraComponent::GetProjectionType() const noexcept
{
    return projectionType;
}

void CameraComponent::SetProjectionType(ProjectionType projectionTypeIn) noexcept
{
    projectionType = projectionTypeIn;
}

void CameraComponent::SetViewportSize(int width, int height) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    viewport.Width = width;
    viewport.Height = height;
}

Matrix4x4 CameraComponent::ComputeProjectionMatrix() const noexcept
{
    if (projectionType == ProjectionType::Orthographic) {
        auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
            viewport.Width,
            viewport.Height,
            distanceToNearPlane,
            distanceToFarPlane);
        return projectionMatrix;
    }

    POMDOG_ASSERT(fieldOfView.value > 0);
    POMDOG_ASSERT(viewport.GetAspectRatio() != 0);
    POMDOG_ASSERT(distanceToNearPlane > 0);
    POMDOG_ASSERT(distanceToFarPlane > 0);

    auto projectionMatrix = Matrix4x4::CreatePerspectiveFieldOfViewLH(
        fieldOfView,
        viewport.GetAspectRatio(),
        distanceToNearPlane,
        distanceToFarPlane);
    return projectionMatrix;
}

std::uint8_t ComponentTypeDeclaration<CameraComponent>::GetTypeIndex()
{
    return Detail::Gameplay::ComponentTypeIndex::Index<CameraComponent>();
}

std::shared_ptr<Component> ComponentCreator<CameraComponent>::CreateComponent()
{
    auto component = std::make_shared<CameraComponent>();
    return component;
}

std::uint8_t ComponentCreator<CameraComponent>::GetComponentType()
{
    return ComponentTypeDeclaration<CameraComponent>::GetTypeIndex();
}

} // namespace Pomdog
