//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "BlendStateGL4.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
class BlendStateGL4::Impl final
{
public:
	
};
//-----------------------------------------------------------------------
BlendStateGL4::BlendStateGL4(BlendDescription const& description)
	: impl(new Impl)
{
}
//-----------------------------------------------------------------------
BlendStateGL4::~BlendStateGL4()
{
}
//-----------------------------------------------------------------------
void BlendStateGL4::Apply()
{
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
