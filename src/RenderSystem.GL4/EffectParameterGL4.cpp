//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "EffectParameterGL4.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Vector4.hpp>
#include <Pomdog/Math/Matrix2x2.hpp>
#include <Pomdog/Math/Matrix3x3.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Quaternion.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
bool EffectParameterGL4::GetValueBool() const
{
	///@todo Not implemented
	return false;
}
//-----------------------------------------------------------------------
std::int32_t EffectParameterGL4::GetValueInt32() const
{
	///@todo Not implemented
	return 0;
}
//-----------------------------------------------------------------------
std::vector<std::int32_t> EffectParameterGL4::GetValueInt32Array() const
{
	///@todo Not implemented
	return std::vector<std::int32_t>{};
}
//-----------------------------------------------------------------------
float EffectParameterGL4::GetValueFloat() const
{
	///@todo Not implemented
	return 0.0f;
}
//-----------------------------------------------------------------------
std::vector<float> EffectParameterGL4::GetValueFloatArray() const
{
	///@todo Not implemented
	return std::vector<float>{};
}
//-----------------------------------------------------------------------
Vector2 EffectParameterGL4::GetValueVector2() const
{
	///@todo Not implemented
	return Vector2(0, 0);
}
//-----------------------------------------------------------------------
std::vector<Vector2> EffectParameterGL4::GetValueVector2Array() const
{
	///@todo Not implemented
	return std::vector<Vector2>{};
}
//-----------------------------------------------------------------------
Vector3 EffectParameterGL4::GetValueVector3() const
{
	///@todo Not implemented
	return Vector3(0, 0, 0);
}
//-----------------------------------------------------------------------
std::vector<Vector3> EffectParameterGL4::GetValueVector3Array() const
{
	///@todo Not implemented
	return std::vector<Vector3>{};
}
//-----------------------------------------------------------------------
Vector4 EffectParameterGL4::GetValueVector4() const
{
	///@todo Not implemented
	return Vector4(0, 0, 0, 0);
}
//-----------------------------------------------------------------------
std::vector<Vector4> EffectParameterGL4::GetValueVector4Array() const
{
	///@todo Not implemented
	return std::vector<Vector4>{};
}
//-----------------------------------------------------------------------
Quaternion EffectParameterGL4::GetValueQuaternion() const
{
	///@todo Not implemented
	return Quaternion::Identity;
}
//-----------------------------------------------------------------------
std::vector<Quaternion> EffectParameterGL4::GetValueQuaternionArray() const
{
	///@todo Not implemented
	return std::vector<Quaternion>{};
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(bool value)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(std::int32_t value)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(float value)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Vector2 const& value)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Vector3 const& value)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Vector4 const& value)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Matrix2x2 const& value)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Matrix3x3 const& value)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Matrix4x4 const& value)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Quaternion const& value)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(float const*, std::size_t)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(std::int32_t const*, std::size_t)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Vector2 const*, std::size_t)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Vector3 const*, std::size_t)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Vector4 const*, std::size_t)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Matrix2x2 const*, std::size_t)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Matrix3x3 const*, std::size_t)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Matrix4x4 const*, std::size_t)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
void EffectParameterGL4::SetValue(Quaternion const*, std::size_t)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
