//
//  Copyright (C) 2013 mogemimi.
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
#include "../RenderSystem/NativeEffectParameter.hpp"
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

	void SetValue(bool) override {}
	void SetValue(std::int32_t) override {}
	void SetValue(float) override {}
	void SetValue(Vector2 const&) override {}
	void SetValue(Vector3 const&) override {}
	void SetValue(Vector4 const&) override {}
	void SetValue(Matrix2x2 const&) override {}
	void SetValue(Matrix3x3 const&) override {}
	void SetValue(Matrix4x4 const&) override {}
	void SetValue(Quaternion const&) override {}
	void SetValue(float const*, std::size_t) override {}
	void SetValue(std::int32_t const*, std::size_t) override {}
	void SetValue(Vector2 const*, std::size_t) override {}
	void SetValue(Vector3 const*, std::size_t) override {}
	void SetValue(Vector4 const*, std::size_t) override {}
	void SetValue(Matrix2x2 const*, std::size_t) override {}
	void SetValue(Matrix3x3 const*, std::size_t) override {}
	void SetValue(Matrix4x4 const*, std::size_t) override {}
	void SetValue(Quaternion const*, std::size_t) override {}
};

}// unnamed namespace
//-----------------------------------------------------------------------
EffectParameter::EffectParameter()
	: nativeEffectParameter(Details::MakeUnique<DummyEffectParameter>())
{
}
//-----------------------------------------------------------------------
EffectParameter::EffectParameter(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
EffectParameter::~EffectParameter()
{
}
//-----------------------------------------------------------------------
EffectAnnotation EffectParameter::GetAnnotation() const
{
	return effectAnnotation;
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
void EffectParameter::SetValue(bool value)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(value);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(std::int32_t value)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(value);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(float value)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(value);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Vector2 const& value)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(value);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Vector3 const& value)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(value);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Vector4 const& value)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(value);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Matrix2x2 const& value)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(value);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Matrix3x3 const& value)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(value);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Matrix4x4 const& value)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(value);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Quaternion const& value)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(value);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Color const& color)
{
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(Vector4{
		color.r, color.g, color.b, color.a
	});
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(float const* source, std::size_t count)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(count > 0);
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(source, count);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(std::int32_t const* source, std::size_t count)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(count > 0);
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(source, count);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Vector2 const* source, std::size_t count)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(count > 0);
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(source, count);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Vector3 const* source, std::size_t count)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(count > 0);
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(source, count);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Vector4 const* source, std::size_t count)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(count > 0);
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(source, count);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Matrix2x2 const* source, std::size_t count)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(count > 0);
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(source, count);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Matrix3x3 const* source, std::size_t count)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(count > 0);
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(source, count);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Matrix4x4 const* source, std::size_t count)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(count > 0);
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(source, count);
}
//-----------------------------------------------------------------------
void EffectParameter::SetValue(Quaternion const* source, std::size_t count)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(count > 0);
	POMDOG_ASSERT(nativeEffectParameter);
	return nativeEffectParameter->SetValue(source, count);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeEffectParameter* EffectParameter::GetNativeEffectParameter()
{
	return nativeEffectParameter.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
