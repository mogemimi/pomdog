// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay2D/Transform.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"

namespace Pomdog {
namespace {

float NormalizeRadian(float radian)
{
    while (radian > Math::TwoPi<float>) {
        radian -= Math::TwoPi<float>;
    }
    while (radian < 0) {
        radian += Math::TwoPi<float>;
    }
    POMDOG_ASSERT(radian >= 0);
    POMDOG_ASSERT(radian <= Math::TwoPi<float>);
    return radian;
}

} // namespace

Transform::Transform() noexcept
    : position(Vector3::Zero)
    , scale(1.0f, 1.0f, 1.0f)
#ifndef POMDOG_TRANSFORM_ROTATION_TYPE_IS_VECTOR3
    , rotation(Quaternion::Identity)
#else
    , rotation(0.0f, 0.0f, 0.0f)
#endif
{
}

Vector2 Transform::GetPosition2D() const noexcept
{
    return Vector2{position.X, position.Y};
}

void Transform::SetPosition2D(const Vector2& positionIn) noexcept
{
    position.X = positionIn.X;
    position.Y = positionIn.Y;
}

const Vector3& Transform::GetPosition() const noexcept
{
    return position;
}

void Transform::SetPosition(const Vector3& positionIn) noexcept
{
    position = positionIn;
}

void Transform::SetPositionX(float x) noexcept
{
    position.X = x;
}

void Transform::SetPositionY(float y) noexcept
{
    position.Y = y;
}

void Transform::SetPositionZ(float z) noexcept
{
    position.Z = z;
}

const Vector3& Transform::GetScale() const noexcept
{
    return scale;
}

void Transform::SetScale(const Vector3& scaleIn) noexcept
{
    scale = scaleIn;
}

void Transform::SetScale(float scaleIn) noexcept
{
    scale.X = scaleIn;
    scale.Y = scaleIn;
    scale.Z = scaleIn;
}

Vector2 Transform::GetScale2D() const noexcept
{
    return Vector2{position.X, position.Y};
}

Radian<float> Transform::GetRotation2D() const noexcept
{
#ifndef POMDOG_TRANSFORM_ROTATION_TYPE_IS_VECTOR3
    return Radian<float>(rotation.W);
#else
    return Radian<float>(Quaternion::CreateFromYawPitchRoll(
        rotation.Y, rotation.X, rotation.Z).W);
#endif
}

Quaternion Transform::GetRotation() const noexcept
{
#ifndef POMDOG_TRANSFORM_ROTATION_TYPE_IS_VECTOR3
    return rotation;
#else
    return Quaternion::CreateFromYawPitchRoll(
        rotation.Y, rotation.X, rotation.Z);
#endif
}

void Transform::SetRotation(const Quaternion& rotationIn) noexcept
{
#if 1
    rotation = rotationIn;
#else
    rotation = Vector3::ToEulerAngles(rotationIn);
#endif
}

void Transform::SetRotationX(const Radian<float>& angle) noexcept
{
#ifndef POMDOG_TRANSFORM_ROTATION_TYPE_IS_VECTOR3
    auto pitchYawRoll = Vector3::ToEulerAngles(rotation);
    pitchYawRoll.X = angle.value;
    rotation = Quaternion::CreateFromYawPitchRoll(
        pitchYawRoll.Y, pitchYawRoll.X, pitchYawRoll.Z);
#else
    rotation.X = angle.value;
#endif
}

void Transform::SetRotationY(const Radian<float>& angle) noexcept
{
#ifndef POMDOG_TRANSFORM_ROTATION_TYPE_IS_VECTOR3
    auto pitchYawRoll = Vector3::ToEulerAngles(rotation);
    pitchYawRoll.Y = angle.value;
    rotation = Quaternion::CreateFromYawPitchRoll(
        pitchYawRoll.Y, pitchYawRoll.X, pitchYawRoll.Z);
#else
    rotation.Y = angle.value;
#endif
}

void Transform::SetRotationZ(const Radian<float>& angle) noexcept
{
#ifndef POMDOG_TRANSFORM_ROTATION_TYPE_IS_VECTOR3
    auto pitchYawRoll = Vector3::ToEulerAngles(rotation);
    pitchYawRoll.Z = angle.value;
    rotation = Quaternion::CreateFromYawPitchRoll(
        pitchYawRoll.Y, pitchYawRoll.X, pitchYawRoll.Z);
#else
    rotation.Z = angle.value;
#endif
}

void Transform::Rotate(const Vector3& eulerAngles)
{
#ifndef POMDOG_TRANSFORM_ROTATION_TYPE_IS_VECTOR3
    const auto pitchYawRoll = Vector3::ToEulerAngles(rotation);
    const auto pitch = NormalizeRadian(pitchYawRoll.X + eulerAngles.X);
    const auto yaw = NormalizeRadian(pitchYawRoll.Y + eulerAngles.Y);
    const auto roll = NormalizeRadian(pitchYawRoll.Z + eulerAngles.Z);
    rotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
#else
    rotation.X = normalize(rotation.X + eulerAngles.X);
    rotation.Y = normalize(rotation.Y + eulerAngles.Y);
    rotation.Z = normalize(rotation.Z + eulerAngles.Z);
#endif
}

void Transform::Rotate2D(const Radian<float>& rotationZ)
{
    rotation = Quaternion::CreateFromAxisAngle(
        Vector3::UnitZ, NormalizeRadian(rotationZ.value));
}

Matrix4x4 Transform::GetTransformMatrix() const noexcept
{
    return Matrix4x4::CreateScale(scale)
        * Matrix4x4::CreateFromQuaternion(rotation)
        * Matrix4x4::CreateTranslation(position);
}

std::uint8_t ComponentTypeDeclaration<Transform>::GetTypeIndex()
{
    return Detail::Gameplay::ComponentTypeIndex::Index<Transform>();
}

std::shared_ptr<Component> ComponentCreator<Transform>::CreateComponent()
{
    auto component = std::make_shared<Transform>();
    return component;
}

std::uint8_t ComponentCreator<Transform>::GetComponentType()
{
    return ComponentTypeDeclaration<Transform>::GetTypeIndex();
}

} // namespace Pomdog
