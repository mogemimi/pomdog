//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_BLENDSTATEGL4_323A6B80_5FD6_11E3_8E58_A8206655A22B_HPP
#define POMDOG_SRC_GL4_BLENDSTATEGL4_323A6B80_5FD6_11E3_8E58_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include "../RenderSystem/NativeBlendState.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class BlendStateGL4 final: public NativeBlendState
{
public:
	BlendStateGL4() = delete;
	
	explicit BlendStateGL4(BlendDescription const& description);
	
	~BlendStateGL4();
	
	void Apply();
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_BLENDSTATEGL4_323A6B80_5FD6_11E3_8E58_A8206655A22B_HPP)
