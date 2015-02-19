//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_DETAIL_GRAPHICS_SHADERBYTECODE_02AC7B6D_D9F3_4354_AFF4_63F439E542DC_HPP
#define POMDOG_DETAIL_GRAPHICS_SHADERBYTECODE_02AC7B6D_D9F3_4354_AFF4_63F439E542DC_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstddef>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class ShaderBytecode {
public:
	void const* Code;
	std::size_t ByteLength;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_GRAPHICS_SHADERBYTECODE_02AC7B6D_D9F3_4354_AFF4_63F439E542DC_HPP)
