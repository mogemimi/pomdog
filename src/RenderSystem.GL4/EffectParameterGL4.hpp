//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_EFFECTPARAMETERGL4_477143C6_EA0B_4118_95F5_1F606E7063A9_HPP
#define POMDOG_SRC_GL4_EFFECTPARAMETERGL4_477143C6_EA0B_4118_95F5_1F606E7063A9_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Graphics/EffectAnnotation.hpp>
#include "../RenderSystem/NativeEffectParameter.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class EffectParameterGL4 final: public NativeEffectParameter
{
public:
	///@copydoc EffectParameter
	EffectAnnotation GetAnnotation() const override;

	///@copydoc EffectParameter
	bool GetValueBool() const override;
	
	///@copydoc EffectParameter
	std::int32_t GetValueInt32() const override;
	
	///@copydoc EffectParameter
	std::vector<std::int32_t> GetValueInt32Array() const override;
	
	///@copydoc EffectParameter
	float GetValueFloat() const override;
	
	///@copydoc EffectParameter
	std::vector<float> GetValueFloatArray() const override;
	
	///@copydoc EffectParameter
	Vector2 GetValueVector2() const override;

	///@copydoc EffectParameter
	std::vector<Vector2> GetValueVector2Array() const override;

	///@copydoc EffectParameter
	Vector3 GetValueVector3() const override;

	///@copydoc EffectParameter
	std::vector<Vector3> GetValueVector3Array() const override;

	///@copydoc EffectParameter
	Vector4 GetValueVector4() const override;

	///@copydoc EffectParameter
	std::vector<Vector4> GetValueVector4Array() const override;

	///@copydoc EffectParameter
	Quaternion GetValueQuaternion() const override;

	///@copydoc EffectParameter
	std::vector<Quaternion> GetValueQuaternionArray() const override;

	///@copydoc EffectParameter
	void SetValue(bool) override;
	
	///@copydoc EffectParameter
	void SetValue(std::int32_t) override;
	
	///@copydoc EffectParameter
	void SetValue(float) override;
	
	///@copydoc EffectParameter
	void SetValue(Vector2 const&) override;
	
	///@copydoc EffectParameter
	void SetValue(Vector3 const&) override;
	
	///@copydoc EffectParameter
	void SetValue(Vector4 const&) override;
	
	///@copydoc EffectParameter
	void SetValue(Matrix2x2 const&) override;
	
	///@copydoc EffectParameter
	void SetValue(Matrix3x3 const&) override;
	
	///@copydoc EffectParameter
	void SetValue(Matrix4x4 const&) override;
	
	///@copydoc EffectParameter
	void SetValue(Quaternion const&) override;
	
	///@copydoc EffectParameter
	void SetValue(float const*, std::size_t) override;
	
	///@copydoc EffectParameter
	void SetValue(std::int32_t const*, std::size_t) override;
	
	///@copydoc EffectParameter
	void SetValue(Vector2 const*, std::size_t) override;
	
	///@copydoc EffectParameter
	void SetValue(Vector3 const*, std::size_t) override;
	
	///@copydoc EffectParameter
	void SetValue(Vector4 const*, std::size_t) override;
	
	///@copydoc EffectParameter
	void SetValue(Matrix2x2 const*, std::size_t) override;
	
	///@copydoc EffectParameter
	void SetValue(Matrix3x3 const*, std::size_t) override;
	
	///@copydoc EffectParameter
	void SetValue(Matrix4x4 const*, std::size_t) override;
	
	///@copydoc EffectParameter
	void SetValue(Quaternion const*, std::size_t) override;

public:
	///@~Japanese
	/// @brief 現在有効になっているシェーダプログラムに定数バッファを適用します。
	void Apply();
	
private:
	EffectAnnotation effectAnnotation;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_EFFECTPARAMETERGL4_477143C6_EA0B_4118_95F5_1F606E7063A9_HPP)
