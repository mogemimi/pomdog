//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_EFFECTBUFFERDESCRIPTION_FA23AA8E_D12D_4A16_AC52_248C895D80BA_HPP
#define POMDOG_SRC_RENDERSYSTEM_EFFECTBUFFERDESCRIPTION_FA23AA8E_D12D_4A16_AC52_248C895D80BA_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <string>
#include <Pomdog/Config/FundamentalTypes.hpp>
#include <Pomdog/Graphics/EffectAnnotation.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class EffectVariableDescription
{
public:
	std::string Name;
	EffectAnnotation Annotation;
	std::uint32_t ByteSize;
};

class EffectBufferDescription
{
public:
	std::vector<EffectVariableDescription> Variables;
	std::string Name;
	std::uint32_t ByteConstants;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_EFFECTBUFFERDESCRIPTION_FA23AA8E_D12D_4A16_AC52_248C895D80BA_HPP)
