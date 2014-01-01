//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/CustomVertex.hpp>
#include <Pomdog/Graphics/VertexElementFormat.hpp>

namespace Pomdog {
namespace Details {
namespace Graphics {

std::uint8_t ToByteWidth(VertexElementFormat format)
{
	switch (format) {
	case VertexElementFormat::Byte4: return 4;
	case VertexElementFormat::Float: return 4;
	case VertexElementFormat::Float2: return 8;
	case VertexElementFormat::Float3: return 12;
	case VertexElementFormat::Float4: return 16;
	case VertexElementFormat::HalfFloat2: return 4;
	case VertexElementFormat::HalfFloat4: return 16;
	}
#ifdef _MSC_VER
	return 4;
#endif
}

}// namespace Graphics
}// namespace Details
}// namespace Pomdog
