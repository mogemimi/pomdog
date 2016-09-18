// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/detail/ComponentTypeIndex.hpp"
#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>

namespace Pomdog {

enum class ProjectionType : std::uint8_t {
    /// @brief The viewing volume in orthographic projection.
    Orthographic,

    /// @brief The viewing volume in perspective projection.
    Perspective,
};

class POMDOG_EXPORT CameraComponent final : public Component {
public:
    CameraComponent() noexcept;

    float GetZoom() const noexcept;

    void SetZoom(float zoomIn) noexcept;

    float GetNear() const noexcept;

    void SetNear(float nearIn) noexcept;

    float GetFar() const noexcept;

    void SetFar(float farIn) noexcept;

    Radian<float> GetFieldOfView() const noexcept;

    void SetFieldOfView(const Radian<float>& fieldOfView) noexcept;

    float GetNormalizedViewportX() const noexcept;

    void SetNormalizedViewportX(float viewportX) noexcept;

    float GetNormalizedViewportY() const noexcept;

    void SetNormalizedViewportY(float viewportY) noexcept;

    float GetNormalizedViewportWidth() const noexcept;

    void SetNormalizedViewportWidth(float viewportWidth) noexcept;

    float GetNormalizedViewportHeight() const noexcept;

    void SetNormalizedViewportHeight(float viewportHeight) noexcept;

    Color GetBackgroundColor() const noexcept;

    void SetBackgroundColor(const Color& color) noexcept;

    ProjectionType GetProjectionType() const noexcept;

    void SetProjectionType(ProjectionType projectionType) noexcept;

    void SetViewportSize(int width, int height) noexcept;

    Matrix4x4 ComputeProjectionMatrix() const noexcept;

private:
    // range: [std::numeric_limits<float>::epsilon(), max()] or (0, max()]
    float zoom;

    Viewport viewport;
    Radian<float> fieldOfView;

    float normalizedViewportX;
    float normalizedViewportY;
    float normalizedViewportWidth;
    float normalizedViewportHeight;

    Color backgroundColor;
    ProjectionType projectionType;
};

template <>
struct ComponentTypeDeclaration<CameraComponent> final {
    static std::uint8_t GetTypeIndex();
};

template <>
class ComponentCreator<CameraComponent> final : public ComponentCreatorBase {
public:
    std::shared_ptr<Component> CreateComponent() override;

    std::uint8_t GetComponentType() override;
};

} // namespace Pomdog
