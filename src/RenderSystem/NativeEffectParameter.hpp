//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTPARAMETER_87DABE60_18BB_4C3B_84CB_7F3A9D85276C_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTPARAMETER_87DABE60_18BB_4C3B_84CB_7F3A9D85276C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include <Pomdog/Config/FundamentalTypes.hpp>
#include <Pomdog/Math/detail/ForwardDeclarations.hpp>
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeEffectParameter
{
public:
	NativeEffectParameter() = default;
	NativeEffectParameter(NativeEffectParameter const&) = delete;
	NativeEffectParameter & operator=(NativeEffectParameter const&) = delete;

	virtual ~NativeEffectParameter() = default;

	///@copydoc EffectParameter
	virtual bool GetValueBool() const = 0;

	///@copydoc EffectParameter
	virtual std::int32_t GetValueInt32() const = 0;

	///@copydoc EffectParameter
	virtual std::vector<std::int32_t> GetValueInt32Array() const = 0;

	///@copydoc EffectParameter
	virtual float GetValueFloat() const = 0;

	///@copydoc EffectParameter
	virtual std::vector<float> GetValueFloatArray() const = 0;

	///@copydoc EffectParameter
	virtual Vector2 GetValueVector2() const = 0;

	///@copydoc EffectParameter
	virtual std::vector<Vector2> GetValueVector2Array() const = 0;

	///@copydoc EffectParameter
	virtual Vector3 GetValueVector3() const = 0;

	///@copydoc EffectParameter
	virtual std::vector<Vector3> GetValueVector3Array() const = 0;

	///@copydoc EffectParameter
	virtual Vector4 GetValueVector4() const = 0;
		
	///@copydoc EffectParameter
	virtual std::vector<Vector4> GetValueVector4Array() const = 0;

	///@copydoc EffectParameter
	virtual Quaternion GetValueQuaternion() const = 0;

	///@copydoc EffectParameter
	virtual std::vector<Quaternion> GetValueQuaternionArray() const = 0;
	
	///@copydoc EffectParameter
	virtual void SetValue(std::uint8_t const* data, std::uint32_t byteLength) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTPARAMETER_87DABE60_18BB_4C3B_84CB_7F3A9D85276C_HPP)
