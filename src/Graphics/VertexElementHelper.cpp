// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/detail/VertexElementHelper.hpp"
#include "Pomdog/Graphics/VertexElementFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {

std::uint8_t VertexElementHelper::ToByteSize(VertexElementFormat format)
{
	static_assert(sizeof(float) == 4, "FUS RO DAH");
	switch (format) {
	case VertexElementFormat::Byte4:
	case VertexElementFormat::Float:
	case VertexElementFormat::HalfFloat2:
		return 4;
	case VertexElementFormat::Float2:
	case VertexElementFormat::HalfFloat4:
		return 8;
	case VertexElementFormat::Float3:
		return 12;
	case VertexElementFormat::Int4:
	case VertexElementFormat::Float4:
		return 16;
	}
#ifdef _MSC_VER
	POMDOG_ASSERT("cannot find format.");
	return 4;
#endif
}

}// namespace Detail
}// namespace Pomdog
