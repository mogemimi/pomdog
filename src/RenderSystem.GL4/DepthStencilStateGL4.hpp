//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_DEPTHSTENCILSTATEGL4_B1C75682_46D9_4523_AE37_B5ED393AC50E_HPP
#define POMDOG_SRC_GL4_DEPTHSTENCILSTATEGL4_B1C75682_46D9_4523_AE37_B5ED393AC50E_HPP

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
	
	///@copydoc NativeDepthStencilState
	void Apply() override;
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_DEPTHSTENCILSTATEGL4_B1C75682_46D9_4523_AE37_B5ED393AC50E_HPP)
