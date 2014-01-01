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
#include "EffectAnnotation.hpp"

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

	explicit EffectParameter(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	~EffectParameter();

	///@~Japanese
	/// @brief エフェクトパラメータのメタデータを取得します。
	EffectAnnotation GetAnnotation() const;

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
	void SetValue(bool value);
	void SetValue(std::int32_t value);
	void SetValue(float value);
	void SetValue(Vector2 const& vector);
	void SetValue(Vector3 const& vector);
	void SetValue(Vector4 const& vector);
	void SetValue(Matrix2x2 const& matrix);
	void SetValue(Matrix3x3 const& matrix);
	void SetValue(Matrix4x4 const& matrix);
	void SetValue(Quaternion const& quaternion);
	void SetValue(Color const& color);
	void SetValue(float const* source, std::size_t count);
	void SetValue(std::int32_t const* source, std::size_t count);
	void SetValue(Vector2 const* vector, std::size_t count);
	void SetValue(Vector3 const* vector, std::size_t count);
	void SetValue(Vector4 const* vector, std::size_t count);
	void SetValue(Matrix2x2 const* matrix, std::size_t count);
	void SetValue(Matrix3x3 const* matrix, std::size_t count);
	void SetValue(Matrix4x4 const* matrix, std::size_t count);
	void SetValue(Quaternion const* quaternion, std::size_t count);
	
public:
	Details::RenderSystem::NativeEffectParameter* GetNativeEffectParameter();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeEffectParameter> nativeEffectParameter;
	EffectAnnotation effectAnnotation;
};
	
/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPARAMETER_6D116595_A4BD_4566_8445_7BD68FF219B7_HPP)
