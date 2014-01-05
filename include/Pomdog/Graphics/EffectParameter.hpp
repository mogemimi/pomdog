//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EFFECTPARAMETER_6D116595_A4BD_4566_8445_7BD68FF219B7_HPP
#define POMDOG_EFFECTPARAMETER_6D116595_A4BD_4566_8445_7BD68FF219B7_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <vector>
#include "../Config/Export.hpp"
#include "../Math/detail/ForwardDeclarations.hpp"
#include "detail/ForwardDeclarations.hpp"
#include "detail/EffectBinaryParameter.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeEffectParameter;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief エフェクトパラメータです。
class POMDOG_EXPORT EffectParameter
{
public:
	EffectParameter();
	EffectParameter(EffectParameter const&) = delete;
	EffectParameter & operator=(EffectParameter const&) = delete;

	EffectParameter(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::uint32_t byteConstants);

	~EffectParameter();

	///@~Japanese
	/// @brief bool 値として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	bool GetValueBool() const;

	///@~Japanese
	/// @brief 32-bit 符号付き整数として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	std::int32_t GetValueInt32() const;

	///@~Japanese
	/// @brief 32-bit 符号付き整数の配列として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	std::vector<std::int32_t> GetValueInt32Array() const;

	///@~Japanese
	/// @brief 32-bit 浮動小数点数として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	float GetValueFloat() const;

	///@~Japanese
	/// @brief 32-bit 浮動小数点数の配列として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	std::vector<float> GetValueFloatArray() const;

	///@~Japanese
	/// @brief Vector2 として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	Vector2 GetValueVector2() const;

	///@~Japanese
	/// @brief Vector2 の配列として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	std::vector<Vector2> GetValueVector2Array() const;

	///@~Japanese
	/// @brief Vector3 として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	Vector3 GetValueVector3() const;

	///@~Japanese
	/// @brief Vector3 の配列として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	std::vector<Vector3> GetValueVector3Array() const;

	///@~Japanese
	/// @brief Vector4 として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	Vector4 GetValueVector4() const;
		
	///@~Japanese
	/// @brief Vector4 の配列として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	std::vector<Vector4> GetValueVector4Array() const;

	///@~Japanese
	/// @brief Quaternion として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	Quaternion GetValueQuaternion() const;

	///@~Japanese
	/// @brief Quaternion の配列として取得します。
	/// @details キャストできなかった場合 std::runtime_error を投げます。
	std::vector<Quaternion> GetValueQuaternionArray() const;
	
	///@~Japanese
	/// @brief エフェクトパラメータに値を設定します。
	template <typename T>
	void SetValue(T const& value)
	{
		Details::EffectBinaryParameter::Set(*this, value);
	}
	
	///@~Japanese
	/// @brief エフェクトパラメータに値を設定します。
	//template <typename T>
	//void SetValue(T const& data, std::uint32_t count)
	//{
	//	POMDOG_ASSERT(count > 0);
	//	Details::EffectBinaryParameter::Set(*this, data, count);
	//}

	///@~Japanese
	/// @brief エフェクトパラメータに値を設定します。
	/// @param data バイナリデータの先頭を表すポインタ
	/// @param byteLength バイト単位のデータサイズ。
	void SetValue(std::uint8_t const* data, std::uint32_t byteLength);

public:
	Details::RenderSystem::NativeEffectParameter* GetNativeEffectParameter();

private:
	std::unique_ptr<Details::RenderSystem::NativeEffectParameter> nativeEffectParameter;
};
	
/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPARAMETER_6D116595_A4BD_4566_8445_7BD68FF219B7_HPP)
