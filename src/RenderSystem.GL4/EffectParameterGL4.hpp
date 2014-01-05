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

#include <memory>
#include "../RenderSystem/NativeEffectParameter.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class ConstantBufferGL4;

class EffectParameterGL4 final: public NativeEffectParameter
{
public:
	EffectParameterGL4() = delete;
	
	explicit EffectParameterGL4(std::uint32_t byteConstant);
	
	~EffectParameterGL4();

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
	void SetValue(std::uint8_t const* data, std::uint32_t byteLength) override;

public:
	///@~Japanese
	/// @brief シェーダプログラムに定数バッファを適用します。
	void Apply(std::uint32_t slotIndex);
	
private:
	std::unique_ptr<ConstantBufferGL4> constantBuffer;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_EFFECTPARAMETERGL4_477143C6_EA0B_4118_95F5_1F606E7063A9_HPP)
