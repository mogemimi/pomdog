//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_DEPTHSTENCILSTATEGL4_8C88FD85_5F33_11E3_A3DA_A8206655A22B_HPP
#define POMDOG_SRC_GL4_DEPTHSTENCILSTATEGL4_8C88FD85_5F33_11E3_A3DA_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include "../RenderSystem/NativeDepthStencilState.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class DepthStencilStateGL4 final: public NativeDepthStencilState
{
public:
	DepthStencilStateGL4() = delete;
	
	explicit DepthStencilStateGL4(DepthStencilDescription const& description);
	
	~DepthStencilStateGL4();
	
	void Apply();
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_DEPTHSTENCILSTATEGL4_8C88FD85_5F33_11E3_A3DA_A8206655A22B_HPP)
