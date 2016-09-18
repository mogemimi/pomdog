// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/detail/ComponentTypeIndex.hpp"
#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Quaternion.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"

namespace Pomdog {

class POMDOG_EXPORT Transform final : public Component {
public:
    Transform() noexcept;

    Vector2 GetPosition2D() const noexcept;

    void SetPosition2D(const Vector2& positionIn) noexcept;

    const Vector3& GetPosition() const noexcept;

    void SetPosition(const Vector3& positionIn) noexcept;

    void SetPositionX(float x) noexcept;

    void SetPositionY(float y) noexcept;

    void SetPositionZ(float z) noexcept;

    const Vector3& GetScale() const noexcept;

    void SetScale(const Vector3& scaleIn) noexcept;

    void SetScale(float scaleIn) noexcept;

    Vector2 GetScale2D() const noexcept;

    Radian<float> GetRotation2D() const noexcept;

    Quaternion GetRotation() const noexcept;

    void SetRotation(const Quaternion& rotationIn) noexcept;

    void SetRotationX(const Radian<float>& angle) noexcept;

    void SetRotationY(const Radian<float>& angle) noexcept;

    void SetRotationZ(const Radian<float>& angle) noexcept;

    void Rotate(const Vector3& eulerAngles);

    void Rotate2D(const Radian<float>& rotationZ);

    Matrix4x4 GetTransformMatrix() const noexcept;

private:
    Vector3 position;
    Vector3 scale;
    Quaternion rotation;
};

template <>
struct ComponentTypeDeclaration<Transform> final {
    static std::uint8_t GetTypeIndex();
};

template <>
class ComponentCreator<Transform> final : public ComponentCreatorBase {
public:
    std::shared_ptr<Component> CreateComponent() override;

    std::uint8_t GetComponentType() override;
};

} // namespace Pomdog
