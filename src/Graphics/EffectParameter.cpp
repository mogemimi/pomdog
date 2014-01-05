//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/EffectParameter.hpp>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Vector4.hpp>
#include <Pomdog/Math/Matrix2x2.hpp>
#include <Pomdog/Math/Matrix3x3.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Quaternion.hpp>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeEffectParameter.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../Utility/MakeUnique.hpp"

namespace Pomdog {
namespace {

class DummyEffectParameter final: public Details::RenderSystem::NativeEffectParameter
{
public:
	bool GetValueBool() const override
	{ return false; }
	
	std::int32_t GetValueInt32() const override
	{ return 0; }
	
	std::vector<std::int32_t> GetValueInt32Array() const override
	{ return std::vector<std::int32_t>{}; }
	
	float GetValueFloat() const override
	{ return 0.0f; }
	
	std::vector<float> GetValueFloatArray() const override
	{ return std::vector<float>{}; }
	
	Vector2 GetValueVector2() const override
	{ return Vector2(0, 0); }

	std::vector<Vector2> GetValueVector2Array() const override
	{ return std::vector<Vector2>{}; }

	Vector3 GetValueVector3() const override
	{ return Vector3(0, 0, 0); }

	std::vector<Vector3> GetValueVector3Array() const override
	{ return std::vector<Vector3>{}; }

	Vector4 GetValueVector4() const override
	{ return Vector4(0, 0, 0, 0); }

	std::vector<Vector4> GetValueVector4Array() const override
	{ return std::vector<Vector4>{}; }

	Quaternion GetValueQuaternion() const override
	{ return Quaternion::Identity; }

	std::vector<Quaternion> GetValueQuaternionArray() const override
	{ return std::vector<Quaternion>{}; }

	void SetValue(std::uint8_t const*, std::uint32_t) override {}
};

}// unnamed namespace
//-----------------------------------------------------------------------
EffectParameter::EffectParameter()
	: nativeEffectParameter(Details::MakeUnique<DummyEffectParameter>())
{
}
//-----------------------------------------------------------------------
EffectParameter::EffectParameter(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::uint32_t byteConstants)
	: nativeEffectParameter(graphicsDevice->GetNativeGraphicsDevice()->CreateEffectParameter(byteConstants))
{
	POMDOG_ASSERT(byteConstants > 0);
}
//-----------------------------------------------------------------------
EffectParameter::~EffectParameter()
{
}
//-----------------------------------------------------------------------
bool EffectParameter::GetValueBool() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueBool();
}
//-----------------------------------------------------------------------
std::int32_t EffectParameter::GetValueInt32() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueInt32();
}
//-----------------------------------------------------------------------
std::vector<std::int32_t> EffectParameter::GetValueInt32Array() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueInt32Array();
}
//-----------------------------------------------------------------------
float EffectParameter::GetValueFloat() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueFloat();
}
//-----------------------------------------------------------------------
std::vector<float> EffectParameter::GetValueFloatArray() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueFloatArray();
}
//-----------------------------------------------------------------------
Vector2 EffectParameter::GetValueVector2() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueVector2();
}
//-----------------------------------------------------------------------
std::vector<Vector2> EffectParameter::GetValueVector2Array() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueVector2Array();
}
//-----------------------------------------------------------------------
Vector3 EffectParameter::GetValueVector3() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueVector3();
}
//-----------------------------------------------------------------------
std::vector<Vector3> EffectParameter::GetValueVector3Array() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueVector3Array();
}
//-----------------------------------------------------------------------
Vector4 EffectParameter::GetValueVector4() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueVector4();
}
//-----------------------------------------------------------------------
std::vector<Vector4> EffectParameter::GetValueVector4Array() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueVector4Array();
}
//-----------------------------------------------------------------------
Quaternion EffectParameter::GetValueQuaternion() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueQuaternion();
}
//-----------------------------------------------------------------------
std::vector<Quaternion> EffectParameter::GetValueQuaternionArray() const
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->GetValueQuaternionArray();
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(std::uint8_t const* data, std::uint32_t byteLength)
{
	POMDOG_ASSERT(data != nullptr);
	POMDOG_ASSERT(byteLength > 0);
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(data, byteLength);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeEffectParameter* EffectParameter::GetNativeEffectParameter()
{
	return nativeEffectParameter.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
