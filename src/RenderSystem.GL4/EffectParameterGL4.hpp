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
	
	explicit EffectParameterGL4(std::uint32_t byteWidth);
	
	~EffectParameterGL4();

	///@copydoc NativeEffectParameter
	void GetValue(std::uint32_t byteWidth, std::uint8_t* result) const override;

	///@copydoc NativeEffectParameter
	void SetValue(std::uint8_t const* data, std::uint32_t byteWidth) override;

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
